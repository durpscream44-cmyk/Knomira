// PLAY-01F: original engine bridge for an explicitly isolated desktop probe.
// The target compiling this file MUST carry KNOMIRA_ISOLATED_TEST_DATA_ROOT.
#include "shell_engine_probe.h"

#include "../../src/base/config.h"
#include "../../src/base/db.h"
#include "../../src/base/global.h"
#include "../../src/base/io.h"
#include "../../src/base/io_data_root.h"
#include "../../src/game/gamemanager.h"
#include "../../src/game/newgamesettings.h"
#include "../../src/gui/eventconnector.h"
#include "../../src/gui/strings.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTemporaryDir>
#include <QThread>

#ifndef KNOMIRA_ISOLATED_TEST_DATA_ROOT
#error The shell-engine probe must never use the production save root
#endif

namespace {
bool fail(QString* error, const QString& detail) {
    if (error) *error = detail;
    return false;
}
}

KnomiraShellEngineProbe::~KnomiraShellEngineProbe() {
    // Manager owns EventConnector and all original aggregator QObjects.
    manager_.reset();
    Global::cfg = nullptr;
    config_.reset();
    root_.reset();
    qunsetenv("KNOMIRA_TEST_DATA_ROOT");
}

bool KnomiraShellEngineProbe::prepare(QString* error) {
    if (manager_) return true;
    if (config_ || root_)
        return fail(error, QStringLiteral("An earlier engine bootstrap failed; restart this probe."));

    qputenv("ALSOFT_DRIVERS", "null");
    root_ = std::make_unique<QTemporaryDir>();
    if (!root_->isValid())
        return fail(error, QStringLiteral("Could not allocate a disposable test directory."));
    QFile marker(root_->filePath(QStringLiteral(".knomira-isolated-test-root")));
    if (!marker.open(QIODevice::WriteOnly) || marker.write("PLAY-01F\n") != 9)
        return fail(error, QStringLiteral("Could not mark the disposable test directory."));
    marker.close();

    const QString root = QDir(root_->path()).canonicalPath();
    if (root.isEmpty() ||
        !qputenv("KNOMIRA_TEST_DATA_ROOT", root.toLocal8Bit()) ||
        Knomira::DataPaths::validateIsolatedTestRoot(root) != root ||
        IO::getDataFolder() != root)
        return fail(error, QStringLiteral("Isolated engine data-root validation failed."));

    config_ = std::make_unique<Config>();
    if (!config_->valid())
        return fail(error, QStringLiteral("Original game configuration did not initialize."));
    Global::cfg = config_.get();
    if (!DB::init())
        return fail(error, QStringLiteral("Original game database did not initialize."));
    DB::initStructs();
    if (!Strings::gi().init())
        return fail(error, QStringLiteral("Original game strings did not initialize."));

    manager_ = std::make_unique<GameManager>();
    if (!manager_->eventConnector() || !Global::newGameSettings ||
        manager_->game() || manager_->thread() != QThread::currentThread())
        return fail(error, QStringLiteral("Original GameManager failed the UI-thread bootstrap."));
    return true;
}

bool KnomiraShellEngineProbe::startNewGame(QString* error) {
    const QString content = qEnvironmentVariable("KNOMIRA_TEST_CONTENT_DIR");
    if (content.isEmpty())
        return fail(error, QStringLiteral(
            "No test tilesheets supplied. Set KNOMIRA_TEST_CONTENT_DIR to authorized local assets."));
    if (!QDir::isAbsolutePath(content) ||
        !QFileInfo::exists(content + QStringLiteral("/tilesheet/default.png")))
        return fail(error, QStringLiteral("Authorized test content directory is invalid."));
    if (!prepare(error))
        return false;
    if (manager_->game())
        return fail(error, QStringLiteral("A test colony is already running."));

    const QString assetDir = QDir(content).canonicalPath();
    if (assetDir.isEmpty())
        return fail(error, QStringLiteral("Test asset directory could not be resolved."));
    for (const auto& row : DB::selectRows("BaseSprites")) {
        const QString tilesheet = row.value("Tilesheet").toString();
        if (tilesheet.isEmpty() || QFileInfo(tilesheet).fileName() != tilesheet ||
            !QFileInfo::exists(assetDir + QStringLiteral("/tilesheet/") + tilesheet))
            return fail(error, QStringLiteral("Required original tilesheet missing or invalid: ") + tilesheet);
    }

    // These are explicit *test-only* embark parameters, not player defaults.
    // A real New Game configuration screen must precede shipping integration.
    config_->set("dataPath", assetDir);
    config_->set("CurrentVersion", QStringLiteral("0.9.0.0"));
    auto* embark = Global::newGameSettings;
    // NewGameSettings setters report *whether a value changed*, not whether
    // the assignment succeeded. A repeated value is valid and must not block.
    const QString colonyName = QStringLiteral("RmlUi Probe Colony");
    const QString colonySeed = QStringLiteral("120531");
    embark->setKingdomName(colonyName);
    embark->setSeed(colonySeed);
    if (embark->kingdomName() != colonyName || embark->seed() != colonySeed)
        return fail(error, QStringLiteral("Test embark values could not be applied."));
    embark->setWorldSize(64);
    embark->setZLevels(130);
    embark->setGround(100);
    embark->setNumGnomes(1);
    embark->setPeaceful(true);
    embark->setOceanSize(0);
    embark->setRivers(0);

    // This calls the same original EventConnector path as PLAY-01E's smoke.
    // Synchronous on the UI thread for this controlled proof; do not claim
    // it is responsive game-thread or world-renderer integration.
    manager_->eventConnector()->onStartNewGame();
    if (!manager_->game())
        return fail(error, QStringLiteral("Original engine did not create a test colony."));
    manager_->eventConnector()->onSetPause(true);
    return true;
}

bool KnomiraShellEngineProbe::hasGame() const {
    return manager_ && manager_->game();
}

QString KnomiraShellEngineProbe::saveRoot() const {
    return root_ && root_->isValid()
        ? root_->path() + QStringLiteral("/save")
        : QString();
}
