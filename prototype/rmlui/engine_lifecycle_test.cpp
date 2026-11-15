// PLAY-01D: original objects, real queued commands, and cooperative shutdown.
// Does not generate a world or open any player save.
#include "../../src/base/config.h"
#include "../../src/base/db.h"
#include "../../src/base/gamestate.h"
#include "../../src/base/global.h"
#include "../../src/base/io.h"
#include "../../src/base/io_data_root.h"
#include "../../src/base/util.h"
#include "../../src/game/gamemanager.h"
#include "../../src/game/newgamesettings.h"
#include "../../src/gui/eventconnector.h"
#include "../../src/gui/strings.h"
#include "../../src/gui/aggregatoragri.h"
#include "../../src/gui/aggregatorcreatureinfo.h"
#include "../../src/gui/aggregatordebug.h"
#include "../../src/gui/aggregatorinventory.h"
#include "../../src/gui/aggregatorloadgame.h"
#include "../../src/gui/aggregatormilitary.h"
#include "../../src/gui/aggregatorneighbors.h"
#include "../../src/gui/aggregatorpopulation.h"
#include "../../src/gui/aggregatorrenderer.h"
#include "../../src/gui/aggregatorselection.h"
#include "../../src/gui/aggregatorsettings.h"
#include "../../src/gui/aggregatorsound.h"
#include "../../src/gui/aggregatorstockpile.h"
#include "../../src/gui/aggregatortileinfo.h"
#include "../../src/gui/aggregatorworkshop.h"

#include <QCoreApplication>
#include <QDebug>
#include <QEventLoop>
#include <QFile>
#include <QTemporaryDir>
#include <QTimer>
#include <cstdio>
#include <cstdlib>
#include <exception>

#ifndef KNOMIRA_ISOLATED_TEST_DATA_ROOT
#error This executable must never use the production data root
#endif

static void require(bool condition, const char* message)
{
    if (!condition)
    {
        std::fprintf(stderr, "Lifecycle regression: %s\n", message);
        std::fflush(stderr);
        std::_Exit(1);
    }
}

int main(int argc, char** argv)
{
    qInstallMessageHandler([](QtMsgType, const QMessageLogContext&, const QString& message) {
        std::fprintf(stderr, "%s\n", message.toLocal8Bit().constData());
        std::fflush(stderr);
    });
    // Report destructor exceptions instead of hanging unattended CI in an MSVC
    // runtime dialog. This is a failure path, never a successful shutdown.
    std::set_terminate([] {
        try {
            if (std::current_exception()) std::rethrow_exception(std::current_exception());
        } catch (const std::exception& error) {
            std::fprintf(stderr, "Lifecycle exception: %s\n", error.what());
        } catch (...) {
            std::fprintf(stderr, "Unknown lifecycle exception\n");
        }
        std::fflush(stderr);
        std::_Exit(1);
    });
    QCoreApplication app(argc, argv);
    QTemporaryDir sandbox;
    require(sandbox.isValid(), "temporary directory unavailable");
    QFile marker(sandbox.filePath(".knomira-isolated-test-root"));
    require(marker.open(QIODevice::WriteOnly) && marker.write("PLAY-01D\n") > 0,
            "could not mark isolated root");
    marker.close();
    const QString root = QDir(sandbox.path()).canonicalPath();
    require(qputenv("KNOMIRA_TEST_DATA_ROOT", root.toLocal8Bit()) &&
            Knomira::DataPaths::validateIsolatedTestRoot(root) == root &&
            IO::getDataFolder() == root, "unsafe test root");

    // OpenAL Soft's actual null output backend allows headless Windows CI.
    // The real sound aggregator/device/context/filter destructors still execute.
    qputenv("ALSOFT_DRIVERS", "null");
    Config config;
    require(config.valid(), "original Config initialization failed");
    Global::cfg = &config;
    require(DB::init(), "original DB initialization failed");
    DB::initStructs();
    require(Strings::gi().init(), "original Strings initialization failed");

    for (int cycle = 0; cycle != 3; ++cycle) {
        qInfo() << "Lifecycle cycle" << cycle << "construct";
        auto* manager = new GameManager;
        QPointer<GameManager> managerGuard(manager);
        QPointer<EventConnector> connector(manager->eventConnector());
        QPointer<NewGameSettings> embark(Global::newGameSettings);
        require(connector && connector == Global::eventConnector &&
                connector->parent() == manager, "connector ownership");
        require(embark && embark->parent() == manager && embark->numGnomes() > 0 &&
                embark->worldSize() > 0, "real embark defaults/ownership");
        require(Global::util && !Global::sel && !manager->game() && !connector->game() &&
                manager->paused() && manager->gameSpeed() == GameSpeed::Normal,
                "startup unexpectedly has a game or stale globals");
        QVariantMap state;
        GameState::serialize(state);
        require(state.value("nextID").toUInt() == 1000000 && GameState::watchedItemList.isEmpty(),
                "original GameState initialization did not execute");
        require(!Global::util->seedItems("Plant").isEmpty(), "real Util/DB unavailable");

        QList<QPointer<QObject>> aggregators {
            connector->aggregatorAgri(), connector->aggregatorCreatureInfo(),
            connector->aggregatorDebug(), connector->aggregatorInventory(),
            connector->aggregatorLoadGame(), connector->aggregatorMilitary(),
            connector->aggregatorNeighbors(), connector->aggregatorPopulation(),
            connector->aggregatorRenderer(), connector->aggregatorSelection(),
            connector->aggregatorSettings(), connector->aggregatorSound(),
            connector->aggregatorStockpile(), connector->aggregatorTileInfo(),
            connector->aggregatorWorkshop()
        };
        for (auto aggregator : aggregators)
            require(aggregator && aggregator->parent() == connector, "aggregator ownership");

        QThread gameThread;
        QEventLoop replies;
        QStringList observed;
        QObject::connect(connector, &EventConnector::signalUpdatePause, &replies,
                         [&](bool paused) { require(paused, "menu must pause"); observed << "pause"; });
        QObject::connect(connector, &EventConnector::signalInMenu, &replies,
                         [&](bool menu) { require(menu, "menu relay"); observed << "menu"; });
        QObject::connect(connector, &EventConnector::signalLoadGameDone, &replies,
                         [&](bool loaded) { require(!loaded, "empty root loaded a save"); observed << "no-save"; });
        QObject::connect(connector, &EventConnector::stopGame, &replies,
                         [&] { observed << "stop"; });
        QObject::connect(connector, &EventConnector::signalExit, &replies,
                         [&] { observed << "exit"; });
        // QObject::deleteLater executes on the game thread before quit/wait.
        // There is deliberately no terminate(), forced process kill or fake bus.
        QObject::connect(manager, &QObject::destroyed, &gameThread, &QThread::quit,
                         Qt::DirectConnection);
        QObject::connect(manager, &QObject::destroyed, [] { qInfo() << "Manager destruction entered"; });
        QObject::connect(&gameThread, &QThread::finished, &replies, &QEventLoop::quit);
        manager->moveToThread(&gameThread);
        require(manager->thread() == &gameThread && connector->thread() == &gameThread &&
                embark->thread() == &gameThread, "QObject tree did not move to game thread");
        for (auto aggregator : aggregators)
            require(aggregator->thread() == &gameThread, "aggregator thread affinity");
        gameThread.start();
        require(QMetaObject::invokeMethod(connector, [manager, connector] {
            qInfo() << "Lifecycle worker commands started";
            require(QThread::currentThread() == manager->thread(), "command thread affinity");
            connector->onSetShowMainMenu(true);
            connector->onSetPause(false);
            connector->onSetGameSpeed(GameSpeed::Normal);
            connector->onHeartbeatResponse(1);
            connector->onSaveGame(); // no active game: must not write
            connector->onContinueLastGame(); // empty isolated root: false
            connector->onEndGame();
            connector->onEndGame(); // idempotent with no active game
            connector->onExit();
            qInfo() << "Lifecycle worker commands complete";
            manager->deleteLater();
        }, Qt::QueuedConnection), "could not queue real connector commands");
        QTimer deadline;
        deadline.setSingleShot(true);
        QObject::connect(&deadline, &QTimer::timeout, &replies,
                         [] { require(false, "shutdown exceeded 10 seconds"); });
        deadline.start(10000);
        replies.exec();
        require(gameThread.wait(5000), "cooperative thread join failed");
        require(observed == QStringList({"pause", "menu", "no-save", "stop", "stop", "exit"}),
                "queued lifecycle replies missing or out of order");
        require(!managerGuard && !connector && !embark, "QObject lifecycle leaked");
        for (auto aggregator : aggregators)
            require(!aggregator, "aggregator outlived its manager");
        require(!Global::eventConnector && !Global::newGameSettings && !Global::sel && !Global::util,
                "shutdown left dangling global pointers");
        require(QDir(root + "/save").isEmpty(), "startup/shutdown wrote a colony save");
    }
    Global::cfg = nullptr;
    qInfo() << "Original GameManager/EventConnector lifecycle passed: 3 threaded cycles, 15 real aggregators, no saves.";
    return 0;
}
