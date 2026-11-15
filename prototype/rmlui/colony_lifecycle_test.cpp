// PLAY-01E: opt-in real colony, timer/heartbeat, pending order and save roundtrip.
// Supply authorized tilesheets via KNOMIRA_TEST_CONTENT_DIR. Never uses player saves.
#include "../../src/base/config.h"
#include "../../src/base/db.h"
#include "../../src/base/gamestate.h"
#include "../../src/base/global.h"
#include "../../src/base/io.h"
#include "../../src/base/io_data_root.h"
#include "../../src/base/selection.h"
#include "../../src/game/game.h"
#include "../../src/game/gamemanager.h"
#include "../../src/game/gnomemanager.h"
#include "../../src/game/gnome.h"
#include "../../src/game/creaturemanager.h"
#include "../../src/game/inventory.h"
#include "../../src/game/job.h"
#include "../../src/game/jobmanager.h"
#include "../../src/game/militarymanager.h"
#include "../../src/game/newgamesettings.h"
#include "../../src/game/plant.h"
#include "../../src/game/world.h"
#include "../../src/gui/eventconnector.h"
#include "../../src/gui/strings.h"

#include <QApplication>
#include <QCryptographicHash>
#include <QDebug>
#include <QDirIterator>
#include <QEventLoop>
#include <QFile>
#include <QTemporaryDir>
#include <QTimer>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <algorithm>

#ifndef KNOMIRA_ISOLATED_TEST_DATA_ROOT
#error Colony tests require the guarded temporary data root
#endif

static void require(bool condition, const char* message)
{
    if (!condition) {
        std::fprintf(stderr, "Colony regression: %s\n", message);
        std::fflush(stderr);
        std::_Exit(1);
    }
}

static QMap<QString, QByteArray> saveHashes(const QString& path)
{
    require(QDir(path).canonicalPath().startsWith(IO::getDataFolder() + "/save/"),
            "save inspection escaped the test root");
    QMap<QString, QByteArray> result;
    QDirIterator files(path, QDir::Files, QDirIterator::Subdirectories);
    while (files.hasNext()) {
        QFile file(files.next());
        require(file.open(QIODevice::ReadOnly), "could not hash saved file");
        result.insert(QDir(path).relativeFilePath(file.fileName()),
                      QCryptographicHash::hash(file.readAll(), QCryptographicHash::Sha256));
    }
    require(result.size() >= 20, "save did not contain the original game collections");
    return result;
}

static void advanceTicks(GameManager& manager, quint64 ticks)
{
    const quint64 target = GameState::tick + ticks;
    QEventLoop loop;
    QTimer timeout;
    timeout.setSingleShot(true);
    QObject::connect(&timeout, &QTimer::timeout, &loop,
                     [] { require(false, "real simulation timer did not progress"); });
    QObject::connect(manager.eventConnector(), &EventConnector::signalHeartbeat, &loop, [&](int) {
        if (GameState::tick >= target) {
            manager.eventConnector()->onSetPause(true);
            loop.quit();
        }
    });
    manager.eventConnector()->onSetPause(false);
    timeout.start(60000);
    loop.exec();
    require(GameState::tick == target && manager.paused(), "unexpected tick/pause state");
}

static void completeTreeOrder(GameManager& manager, unsigned int order, unsigned int tree)
{
    QEventLoop loop;
    QTimer timeout;
    timeout.setSingleShot(true);
    QObject::connect(&timeout, &QTimer::timeout, &loop,
                     [] { require(false, "settler did not complete the restored tree order in 60 seconds"); });
    const quint64 startTick = GameState::tick;
    QObject::connect(manager.eventConnector(), &EventConnector::signalHeartbeat, &loop, [&](int) {
        require(GameState::tick - startTick < 2000, "restored order exceeded 2000 simulation ticks");
        if (!manager.game()->jm()->allJobs().contains(order)) {
            manager.eventConnector()->onSetPause(true);
            loop.quit();
        }
    });
    manager.eventConnector()->onSetGameSpeed(GameSpeed::Fast);
    require(manager.gameSpeed() == GameSpeed::Fast, "real fast-speed command failed");
    manager.eventConnector()->onSetPause(false);
    timeout.start(60000);
    loop.exec();
    require(!manager.game()->w()->plants().contains(tree), "restored order disappeared without felling the tree");
}

int main(int argc, char** argv)
{
    qInstallMessageHandler([](QtMsgType type, const QMessageLogContext&, const QString& message) {
        std::fprintf(stderr, "%s\n", message.toLocal8Bit().constData());
        std::fflush(stderr);
        if (type == QtFatalMsg) std::_Exit(1);
    });
    std::set_terminate([] {
        try {
            if (std::current_exception()) std::rethrow_exception(std::current_exception());
        } catch (const std::exception& error) {
            std::fprintf(stderr, "Colony exception: %s\n", error.what());
        } catch (...) { std::fprintf(stderr, "Unknown colony exception\n"); }
        std::fflush(stderr);
        std::_Exit(1);
    });
    qputenv("QT_QPA_PLATFORM", "offscreen");
    qputenv("ALSOFT_DRIVERS", "null");
    QApplication app(argc, argv);
    const QString content = qEnvironmentVariable("KNOMIRA_TEST_CONTENT_DIR");
    if (content.isEmpty()) {
        qInfo("SKIP: set KNOMIRA_TEST_CONTENT_DIR to an authorized content directory with tilesheets");
        return 77;
    }
    require(QDir::isAbsolutePath(content) && QFileInfo::exists(content + "/tilesheet/default.png"),
            "explicit content directory is invalid");
    QTemporaryDir sandbox;
    require(sandbox.isValid(), "temporary root unavailable");
    QFile marker(sandbox.filePath(".knomira-isolated-test-root"));
    require(marker.open(QIODevice::WriteOnly) && marker.write("PLAY-01E\n") > 0, "root marker failed");
    marker.close();
    const QString root = QDir(sandbox.path()).canonicalPath();
    require(qputenv("KNOMIRA_TEST_DATA_ROOT", root.toLocal8Bit()) &&
            Knomira::DataPaths::validateIsolatedTestRoot(root) == root && IO::getDataFolder() == root,
            "unsafe test data root");
    Config config;
    require(config.valid(), "Config startup failed");
    Global::cfg = &config;
    require(DB::init(), "DB startup failed");
    DB::initStructs();
    require(Strings::gi().init(), "Strings startup failed");
    // Only read this external asset directory. All persisted state uses the
    // separately guarded IO root, including any automatic save from Game::start.
    config.set("dataPath", content);
    config.set("CurrentVersion", "0.9.0.0");
    for (const auto& row : DB::selectRows("BaseSprites"))
        require(QFileInfo::exists(content + "/tilesheet/" + row.value("Tilesheet").toString()),
                "a DB-referenced tilesheet is missing");

    {
        GameManager manager;
        auto* connector = manager.eventConnector();
        QObject::connect(connector, &EventConnector::signalHeartbeat,
                         connector, &EventConnector::onHeartbeatResponse);
        int starts = 0;
        int loads = 0;
        QObject::connect(connector, &EventConnector::startGame, &app, [&] { ++starts; });
        QObject::connect(connector, &EventConnector::signalLoadGameDone, &app,
                         [&](bool success) { require(success, "real save load failed"); ++loads; });
        auto* settings = Global::newGameSettings;
        settings->setSeed("120531");
        settings->setKingdomName("Lifecycle Colony");
        settings->setWorldSize(64);
        settings->setZLevels(130);
        settings->setGround(100);
        settings->setNumGnomes(1);
        settings->setPeaceful(true);
        settings->setOceanSize(0);
        settings->setRivers(0);
        std::srand(120531);
        connector->onStartNewGame();
        auto* game = manager.game();
        require(starts == 1 && game && game->w() && connector->game() == game &&
                game->w()->world().size() == 64 * 64 * 130 && game->gm()->gnomes().size() == 1,
                "real New Game did not create a populated world");
        require(game->mil()->squads().size() == 1, "default squad initialization failed");
        const auto firstSquad = game->mil()->squads().first().id;
        const auto secondSquad = game->mil()->addSquad();
        game->mil()->moveSquadUp(secondSquad);
        require(game->mil()->squads().first().id == secondSquad, "squad reordering failed");
        advanceTicks(manager, 3);
        // Pause must prevent timer callbacks from advancing the simulation.
        const quint64 pausedTick = GameState::tick;
        QEventLoop pausedWait;
        QTimer::singleShot(150, &pausedWait, &QEventLoop::quit);
        pausedWait.exec();
        require(GameState::tick == pausedTick, "pause did not stop simulation progression");

        QList<unsigned int> trees;
        const Position settler = game->gm()->gnomes().first()->getPos();
        for (auto it = game->w()->plants().begin(); it != game->w()->plants().end(); ++it)
            if (it.value().matureWood() && it.value().getPos().z == settler.z) trees.append(it.key());
        std::sort(trees.begin(), trees.end(), [&](unsigned int a, unsigned int b) {
            const int da = Position(a).distSquare(settler);
            const int db = Position(b).distSquare(settler);
            return da == db ? a < b : da < db;
        });
        require(!trees.isEmpty(), "fixed-seed world has no tree for order regression");
        const unsigned int tree = trees.first();
        connector->onTerrainCommand(tree, "Fell");
        auto order = game->jm()->getJobAtPos(Position(tree));
        require(!order.isNull(), "real terrain command failed to create a job");
        const auto orderID = order->id();
        order.clear();
        const auto tiles = game->w()->world().size();
        const auto items = game->inv()->allItems().size();
        const auto plants = game->w()->plants().size();
        const auto creatures = game->cm()->count();
        const auto tick = GameState::tick;
        QPointer<Game> oldGame(game);
        QPointer<Selection> oldSelection(Global::sel);
        // Save through the same bus as the UI while running; it must restore
        // the previous running state, then load the paused snapshot explicitly.
        connector->onSetPause(false);
        connector->onSaveGame();
        require(!manager.paused(), "save failed to restore running state");
        connector->onSetPause(true);
        const QString slot = root + "/save/LifecycleColony/1";
        const auto hashes = saveHashes(slot);
        require(hashes.contains("world.dat") && hashes.contains("game.json"), "world save missing");
        connector->onLoadGame(slot);
        game = manager.game();
        require(!oldGame && !oldSelection && starts == 2 && loads == 1 && game && Global::sel &&
                game->w()->world().size() == tiles && game->gm()->gnomes().size() == 1 &&
                game->inv()->allItems().size() == items && game->w()->plants().size() == plants &&
                game->cm()->count() == creatures && GameState::tick == tick,
                "save/reload changed world population, inventory or clock");
        require(!game->jm()->getJob(orderID).isNull(), "pending terrain job lost on reload");
        require(game->mil()->squads().size() == 2 && game->mil()->squads()[0].id == secondSquad &&
                game->mil()->squads()[1].id == firstSquad, "squad identity/order lost on reload");
        require(saveHashes(slot) == hashes, "loading modified the saved files");
        const auto woodBefore = game->inv()->itemCount("RawWood", "any");
        advanceTicks(manager, 2);
        completeTreeOrder(manager, orderID, tree);
        require(game->inv()->itemCount("RawWood", "any") > woodBefore,
                "completed tree order produced no usable wood");
        connector->onSaveGame();
        require(QFileInfo::exists(root + "/save/LifecycleColony/2/world.dat") &&
                saveHashes(slot) == hashes, "second save overwrote the original slot");
        oldGame = game;
        oldSelection = Global::sel;
        connector->onEndGame();
        connector->onEndGame();
        require(!oldGame && !oldSelection && !manager.game() && !connector->game() &&
                !Global::sel && Global::util, "ending a colony left live session objects");
        qInfo() << "Original colony lifecycle passed:" << tiles << "tiles," << items << "items,"
                << plants << "plants," << creatures << "creatures; restored job completed, wood produced, two save slots preserved.";
    }
    require(!Global::util && !Global::sel && !Global::eventConnector && !Global::newGameSettings,
            "manager teardown left globals after active-colony test");
    Global::cfg = nullptr;
    return 0;
}
