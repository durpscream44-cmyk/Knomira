// PLAY-01B: link and execute original Config + IO initialization in a marked,
// disposable data root. This executable intentionally does not link GameManager.
#include "../../src/base/config.h"
#include "../../src/base/global.h"
#include "../../src/base/io.h"
#include "../../src/base/io_data_root.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTemporaryDir>

// Config/IO::saveConfig only use this original Global static pointer.
// The full Global/game simulation is NOT replaced by mocks here.
Config* Global::cfg = nullptr;

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    QTemporaryDir sandbox;
    if (!sandbox.isValid()) {
        qCritical() << "Unable to create isolated startup directory";
        return 1;
    }

    QFile marker(sandbox.filePath(QStringLiteral(".knomira-isolated-test-root")));
    if (!marker.open(QIODevice::WriteOnly) ||
        marker.write("Marked Knomira test directory\n") <= 0) {
        qCritical() << "Unable to mark isolated startup directory";
        return 2;
    }
    marker.close();

    const QString root = QDir(sandbox.path()).canonicalPath();
    if (!qputenv("KNOMIRA_TEST_DATA_ROOT", root.toLocal8Bit()) ||
        Knomira::DataPaths::validateIsolatedTestRoot(root) != root ||
        IO::getDataFolder() != root) {
        qCritical() << "Unsafe data root: refusing original Config initialization";
        return 3;
    }

    const QDir packaged(QCoreApplication::applicationDirPath() +
                         QStringLiteral("/content/JSON"));
    for (const QString& name : {QStringLiteral("config.json"),
                                QStringLiteral("newgame.json"),
                                QStringLiteral("profs.json")}) {
        if (!QFileInfo::exists(packaged.filePath(name))) {
            qCritical() << "Original configuration asset not staged:" << name;
            return 4;
        }
    }

    Config config; // Actual original Config::Config -> IO::createFolders.
    if (!config.valid()) {
        qCritical() << "Original Config constructor could not load game defaults";
        return 5;
    }
    Global::cfg = &config;
    const QDir data(root);
    for (const QString& name : {QStringLiteral("config.json"),
                                QStringLiteral("newgame.json"),
                                QStringLiteral("profs.json")}) {
        if (!QFileInfo::exists(data.filePath(QStringLiteral("settings/") + name))) {
            qCritical() << "Original Config failed to install default:" << name;
            Global::cfg = nullptr;
            return 6;
        }
    }

    if (config.get(QStringLiteral("dataPath")).toString() !=
        QCoreApplication::applicationDirPath() + QStringLiteral("/content")) {
        qCritical() << "Configured game-data path is not the packaged content";
        Global::cfg = nullptr;
        return 7;
    }

    // Exercise real Config::set + original IO::saveConfig/IO::saveFile.
    constexpr int changed = 287;
    config.set(QStringLiteral("XpMod"), changed);
    QFile settings(data.filePath(QStringLiteral("settings/config.json")));
    if (!settings.open(QIODevice::ReadOnly)) {
        qCritical() << "Could not read isolated config.json after real Config::set";
        Global::cfg = nullptr;
        return 8;
    }
    const QJsonDocument saved = QJsonDocument::fromJson(settings.readAll());
    settings.close();
    if (!saved.isObject() ||
        saved.object().value(QStringLiteral("XpMod")).toInt(-1) != changed) {
        qCritical() << "Original Config::set did not persist the changed setting";
        Global::cfg = nullptr;
        return 9;
    }

    Config reload;
    if (!reload.valid() ||
        reload.get(QStringLiteral("XpMod")).toInt(-1) != changed) {
        qCritical() << "Original Config constructor did not reload saved setting";
        Global::cfg = nullptr;
        return 10;
    }

    if (!QDir(data.filePath(QStringLiteral("save")))
             .entryList(QDir::AllEntries | QDir::NoDotAndDotDot).isEmpty()) {
        qCritical() << "Config initialization unexpectedly created game saves";
        Global::cfg = nullptr;
        return 11;
    }

    Global::cfg = nullptr;
    qInfo() << "Original Config startup, JSON save and reload succeeded in isolated test root";
    return 0;
}
