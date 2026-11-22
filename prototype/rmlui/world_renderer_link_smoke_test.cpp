// PLAY-01H-00: link the real world renderer without starting graphics or a colony.
// This is strictly an asset-free, disposable-root Qt signal/constructor smoke.
#include "shell_engine_probe.h"
#include "../../src/base/global.h"
#include "../../src/base/io.h"
#include "../../src/gui/eventconnector.h"
#include "../../src/gui/mainwindowrenderer.h"

#include <QApplication>
#include <QDebug>
#include <QFileInfo>

#ifndef KNOMIRA_ISOLATED_TEST_DATA_ROOT
#error The renderer link smoke must never use production game data
#endif

int main(int argc, char** argv) {
    // Same headless platform as the verified engine preflight. No QWindow,
    // QOpenGLContext, GLAD initialization, or external tilesheets are used.
    qputenv("QT_QPA_PLATFORM", "offscreen");
    qunsetenv("KNOMIRA_TEST_CONTENT_DIR");
    QApplication application(argc, argv);

    QString disposableRoot;
    {
        KnomiraShellEngineProbe probe;
        QString error;
        if (!probe.prepare(&error)) {
            qCritical() << "Renderer link preflight failed:" << error;
            return 1;
        }
        disposableRoot = IO::getDataFolder();
        if (disposableRoot.isEmpty() ||
            !QFileInfo::exists(disposableRoot +
                QStringLiteral("/.knomira-isolated-test-root")) ||
            !Global::eventConnector || probe.hasGame()) {
            qCritical() << "Renderer link preflight escaped isolated startup";
            return 2;
        }

        // Null host pointers are safe here because the constructor only
        // installs QObject signal connections. Never call initializeGL,
        // resize, cleanupWorld, paintWorld, or any GPU-facing method.
        {
            MainWindowRenderer renderer(nullptr, nullptr);
            if (!renderer.isInMenu()) {
                qCritical() << "Renderer did not start in menu mode";
                return 3;
            }
            Global::eventConnector->emitInMenu(false);
            if (renderer.isInMenu()) {
                qCritical() << "Real EventConnector did not reach renderer";
                return 4;
            }
            Global::eventConnector->emitInMenu(true);
            if (!renderer.isInMenu()) {
                qCritical() << "Renderer did not receive return-to-menu signal";
                return 5;
            }
        } // Disconnect renderer before destroying the original engine.

        if (probe.hasGame()) {
            qCritical() << "Renderer link smoke unexpectedly generated a colony";
            return 6;
        }
    }
    if (QFileInfo::exists(disposableRoot) || Global::eventConnector ||
        Global::newGameSettings || Global::cfg) {
        qCritical() << "Renderer link smoke leaked test root or engine globals";
        return 7;
    }
    qInfo() << "Original renderer links, receives EventConnector signals, and tears down without GL or saves";
    return 0;
}
