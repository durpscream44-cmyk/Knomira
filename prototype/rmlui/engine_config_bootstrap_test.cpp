// Copyright (C) 2026 Knomira contributors.
// PLAY-01B: safely construct the original Config class in an isolated test
// folder; do not link gameplay world, real save serialization, or UI.
#include "../../src/base/config.h"
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

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    QTemporaryDir sandbox;
    if (!sandbox.isValid())
        return 1;

    QFile marker(sandbox.filePath(QStringLiteral(".knomira-isolated-test-root")));
    if (!marker.open(QIODevice::WriteOnly) ||
        marker.write("Knomira test sandbox\n") <= 0)
        return 2;
    marker.close();
    if (!qputenv("KNOMIRA_TEST_DATA_ROOT", sandbox.path().toLocal8Bit()))
        return 3;

    const QString expected = QDir(sandbox.path()).canonicalPath();
    if (IO::getDataFolder() != expected)
        return 4;

    // This is the original constructor, not a fake config fixture.
    // It calls the real isolated IO::createFolders(), then reads the
    // real staged content/JSON/config.json to populate m_settings.
    Config config;
    if (!config.valid()) {
        qCritical() << "Original Config initialization failed";
        return 5;
    }
    if (config.get(QStringLiteral("dataPath")).toString() !=
        QCoreApplication::applicationDirPath() + QStringLiteral("/content")) {
        qCritical() << "Original Config did not resolve packaged content path";
        return 6;
    }
    if (config.get(QStringLiteral("AutoSaveInterval")).toInt() <= 0 ||
        config.get(QStringLiteral("XpMod")).toDouble() <= 0 ||
        !config.get(QStringLiteral("uiscale")).isValid()) {
        qCritical() << "Original Config missing essential defaults";
        return 7;
    }

    const QDir root(expected);
    for (const QString& item : {QStringLiteral("mods"), QStringLiteral("save"),
                                QStringLiteral("screenshots"), QStringLiteral("settings"),
                                QStringLiteral("tmp")}) {
        if (!root.exists(item))
            return 8;
    }
    QJsonDocument onDisk;
    if (!IO::loadFile(root.filePath(QStringLiteral("settings/config.json")), onDisk) ||
        !onDisk.isObject() || onDisk.object().isEmpty())
        return 9;

    QJsonDocument bundled;
    if (!IO::loadOriginalConfig(bundled) ||
        !bundled.isObject() || bundled.object().isEmpty())
        return 10;

    const QFileInfo normalSettings(root.filePath(QStringLiteral("settings/config.json")));
    if (!normalSettings.exists() ||
        !normalSettings.canonicalFilePath().startsWith(expected + QLatin1Char('/')))
        return 11;

    qInfo() << "Original Config constructed successfully using only temporary test data.";
    return 0;
}
