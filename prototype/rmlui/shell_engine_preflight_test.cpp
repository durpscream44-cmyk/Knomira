// PLAY-01F: real-engine bootstrap test with zero external tilesheets or saves.
// No frontend renderer; verifies a disposable, empty GameManager session.
#include "shell_engine_probe.h"
#include "../../src/base/global.h"
#include "../../src/base/io.h"
#include "../../src/game/gamemanager.h"
#include "../../src/gui/eventconnector.h"

#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <cstdlib>

#ifndef KNOMIRA_ISOLATED_TEST_DATA_ROOT
#error This test may never use production data
#endif

static void require(bool ok, const char* message) {
    if (!ok) {
        qCritical() << message;
        std::_Exit(1);
    }
}

int main(int argc, char** argv) {
    qputenv("QT_QPA_PLATFORM", "offscreen");
    QApplication app(argc, argv);

    QString tempRoot;
    {
        KnomiraShellEngineProbe probe;
        QString error;
        require(probe.prepare(&error), qPrintable(error));
        tempRoot = IO::getDataFolder();
        require(!tempRoot.isEmpty() && QFileInfo::exists(
            tempRoot + QStringLiteral("/.knomira-isolated-test-root")),
            "test-only data root marker missing");
        require(!probe.hasGame() && !probe.saveRoot().isEmpty(),
                "preflight generated a world or lacks isolated saves");
        require(Global::eventConnector && Global::newGameSettings,
                "original engine/embark tree was not initialized");
        require(QDir(probe.saveRoot()).entryList(
                    QDir::Dirs | QDir::NoDotAndDotDot).isEmpty(),
                "preflight wrote a colony");
        qInfo() << "Original GameManager connected to isolated shell session; no world or save written";
    }
    require(!Global::eventConnector && !Global::newGameSettings &&
                !Global::util && !Global::cfg,
            "engine session teardown left dangling globals");
    require(!QFileInfo::exists(tempRoot), "disposable root survived teardown");
    return 0;
}
