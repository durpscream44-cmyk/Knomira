// PLAY-01F: real-engine bootstrap test with zero external tilesheets or saves.
// No frontend renderer; verifies a disposable, empty GameManager session.
#include "shell_engine_probe.h"
#include "input_bindings.h"
#include "../../src/base/global.h"
#include "../../src/base/io.h"
#include "../../src/game/gamemanager.h"
#include "../../src/gui/eventconnector.h"

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTemporaryDir>
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
        qunsetenv("KNOMIRA_TEST_CONTENT_DIR");
        QString refused;
        require(!probe.startNewGame(&refused) &&
                    refused.contains(QStringLiteral("KNOMIRA_TEST_CONTENT_DIR")) &&
                    !probe.hasGame(),
                "New Game did not refuse missing authorized tilesheets");

        // Isolated negative proof: a placeholder (NOT a real/third-party
        // tilesheet) gets past only the initial filename existence check;
        // missing public GLSL must refuse before any sprite DB reads or
        // world generation. No real player data is involved.
        {
            QTemporaryDir fakeContent;
            require(fakeContent.isValid() &&
                        QDir().mkpath(fakeContent.filePath(QStringLiteral("tilesheet"))),
                    "Could not prepare isolated fake test-content directory");
            QFile placeholder(fakeContent.filePath(QStringLiteral("tilesheet/default.png")));
            require(placeholder.open(QIODevice::WriteOnly) &&
                        placeholder.write("placeholder, not licensed art") > 0,
                    "Could not write harmless fake tilesheet filename");
            placeholder.close();
            require(qputenv("KNOMIRA_TEST_CONTENT_DIR",
                            QDir(fakeContent.path()).canonicalPath().toLocal8Bit()),
                    "Could not set isolated fake test-content path");
            QString shaderError;
            require(!probe.startNewGame(&shaderError) &&
                        shaderError.contains(QStringLiteral("shaders/world_v.glsl")) &&
                        !probe.hasGame(),
                    "New Game did not refuse missing required world shader");
            qunsetenv("KNOMIRA_TEST_CONTENT_DIR");
        }

        // The exact input profile used by the probe must NEVER access the
        // user's AppConfigLocation. Save and reload only beneath test root.
        const QString controlFile = KnomiraInput::Profile::configPath();
        require(controlFile.startsWith(tempRoot + QStringLiteral("/")),
                "probe controls path escaped the disposable root");
        KnomiraInput::Profile controls;
        require(controls.save(&error) && QFileInfo::exists(controlFile),
                "probe did not persist its controls inside disposable root");
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
