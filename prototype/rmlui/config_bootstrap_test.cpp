// Copyright (C) 2026 Knomira contributors.
// PLAY-01B: instantiate the REAL original Config class with real IO JSON
// functions, but keep all resulting settings and save directories temporary.
// This is not a simulated GameManager and does not create a game world.
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

// Minimal static member definition for the single global pointer actually
// referenced by Config/IO::saveConfig. We do NOT link Global.cpp or create
// fake GameManager, simulation, or UI objects.
Config* Global::cfg = nullptr;

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    QTemporaryDir sandbox;
    if (!sandbox.isValid()) {
        qCritical() << "Failed to allocate isolated startup sandbox";
        return 1;
    }

    QFile marker(sandbox.filePath(QStringLiteral(".knomira-isolated-test-root")));
    if (!marker.open(QIODevice::WriteOnly) ||
        marker.write("Knomira isolated runtime config test\n") <= 0) {
        qCritical() << "Failed to mark isolated sandbox";
        return 2;
    }
    marker.close();

    const QString root = QDir(sandbox.path()).canonicalPath();
    if (!qputenv("KNOMIRA_TEST_DATA_ROOT", root.toLocal8Bit()) ||
        Knomira::DataPaths::validateIsolatedTestRoot(root) != root ||
        IO::getDataFolder() != root) {
        qCritical() << "Unsafe data root; refusing to construct real Config";
        return 3;
    }

    // Original Config::Config invokes IO::createFolders, which is why a
    // smoke test must not run against the user's actual save root.
    Config config;
    if (!config.valid()) {
        qCritical() << "Original game Config failed to load from staged defaults";
        return 4;
    }
    Global::cfg = &config;

    const QDir dataRoot(root);
    for (const QString& file : {QStringLiteral("config.json"),
                                QStringLiteral("profs.json"),
                                QStringLiteral("newgame.json")}) {
        if (!QFileInfo::exists(dataRoot.filePath(QStringLiteral("settings/") + file))) {
            qCritical() << "Config construction failed to stage original default" << file;
            Global::cfg = nullptr;
            return 5;
        }
    }
    const QString stagedData = QCoreApplication::applicationDirPath() +
                               QStringLiteral("/content");
    if (config.get(QStringLiteral("dataPath")).toString() != stagedData) {
        qCritical() << "Config points to wrong game asset directory";
        Global::cfg = nullptr;
        return 6;
    }
    if (config.get(QStringLiteral("language")).toString().isEmpty()) {
        qCritical() << "Config has no original language setting";
        Global::cfg = nullptr;
        return 7;
    }

    // Exercise real Config::set -> IO::saveConfig -> IO::saveFile. Changes
    // must only appear in the QTemporaryDir. Check that they survive a
    // subsequent *real* configuration constructor (load path).
    constexpr int testValue = 287;
    config.set(QStringLiteral("XpMod"), testValue);
    QFile persisted(dataRoot.filePath(QStringLiteral("settings/config.json")));
    if (!persisted.open(QIODevice::ReadOnly)) {
        qCritical() << "Real Config did not persist isolated settings";
        Global::cfg = nullptr;
        return 8;
    }
    const QJsonDocument saved = QJsonDocument::fromJson(persisted.readAll());
    persisted.close();
    if (!saved.isObject() ||
        saved.object().value(QStringLiteral("XpMod")).toInt(-1) != testValue) {
        qCritical() << "Config persistence wrote unexpected data";
        Global::cfg = nullptr;
        return 9;
    }
    {
        Config reloaded;
        if (!reloaded.valid() ||
            reloaded.get(QStringLiteral("XpMod")).toInt() != testValue) {
            qCritical() << "Real Config reload failed in isolated sandbox";
            Global::cfg = nullptr;
            return 10;
        }
    }
    if (!dataRoot.entryList(QDir::Dirs | QDir::NoDotAndDotDot).contains(
            QStringLiteral("save")) ||
        !QDir(dataRoot.filePath(QStringLiteral("save"))).isEmpty()) {
        qCritical() << "Config bootstrap unexpectedly populated the save directory";
        Global::cfg = nullptr;
        return 11;
    }

    Global::cfg = nullptr;
    qInfo() << "Real Config/IO bootstrap and reload passed; all writes were sandboxed.";
    return 0;
}
