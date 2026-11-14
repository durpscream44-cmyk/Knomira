// Copyright (C) 2026 Knomira contributors.
// Runtime smoke test: actual embark/NewGameSettings, DB, Strings and Config.
// No world generation; all settings writes remain in a marked temporary root.
#include "../../src/base/config.h"
#include "../../src/base/db.h"
#include "../../src/base/global.h"
#include "../../src/base/io.h"
#include "../../src/base/io_data_root.h"
#include "../../src/game/newgamesettings.h"
#include "../../src/gui/strings.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTemporaryDir>

Config* Global::cfg = nullptr;
int Global::dimX = 100;
int Global::dimY = 100;
int Global::dimZ = 100;

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    QTemporaryDir sandbox;
    if (!sandbox.isValid())
        return 1;

    QFile marker(sandbox.filePath(QStringLiteral(".knomira-isolated-test-root")));
    if (!marker.open(QIODevice::WriteOnly) ||
        marker.write("Knomira isolated embark test\n") <= 0)
        return 2;
    marker.close();

    const QString isolatedRoot = QDir(sandbox.path()).canonicalPath();
    if (!qputenv("KNOMIRA_TEST_DATA_ROOT", isolatedRoot.toLocal8Bit()) ||
        Knomira::DataPaths::validateIsolatedTestRoot(isolatedRoot) != isolatedRoot ||
        IO::getDataFolder() != isolatedRoot) {
        qCritical() << "Unsafe embark test directory; refusing to initialize";
        return 3;
    }

    Config config;
    if (!config.valid())
        return 4;
    Global::cfg = &config;

    DB::init();
    if (DB::numRows(QStringLiteral("Items")) < 1 ||
        DB::selectRows(QStringLiteral("Items_Components"), QStringLiteral("Pickaxe")).size() != 2) {
        qCritical() << "Original game item/component data missing";
        Global::cfg = nullptr;
        return 5;
    }
    DB::initStructs();
    if (!Strings::gi().init()) {
        Global::cfg = nullptr;
        return 6;
    }

    const QString path = QDir(isolatedRoot).filePath(QStringLiteral("settings/newgame.json"));
    if (!QFileInfo::exists(path)) {
        qCritical() << "Original default embark settings were not staged";
        Global::cfg = nullptr;
        return 7;
    }

    constexpr int uniqueAmount = 53;
    {
        NewGameSettings embark;
        if (embark.worldSize() <= 0 || embark.zLevels() <= 0 ||
            embark.numGnomes() <= 0 || embark.kingdomName().isEmpty() ||
            embark.seed().isEmpty() || embark.presetNames().isEmpty()) {
            qCritical() << "Original new game setup missing essential defaults";
            Global::cfg = nullptr;
            return 8;
        }
        // Tests the actual two-material starting-item persistence path.
        // Before the fix, setStartingItems() reads Components[0] twice.
        embark.addStartingItem(QStringLiteral("Pickaxe"),
                              QStringLiteral("Iron"),
                              QStringLiteral("Oak"),
                              uniqueAmount);
        embark.save();
    }

    QJsonDocument saved;
    if (!IO::loadFile(path, saved) || !saved.isObject()) {
        qCritical() << "Embark settings failed to save in temporary sandbox";
        Global::cfg = nullptr;
        return 9;
    }
    bool foundSerialized = false;
    const QJsonArray items = saved.object().value(QStringLiteral("startingItems")).toArray();
    for (const auto& value : items) {
        const QJsonObject item = value.toObject();
        if (item.value(QStringLiteral("ItemID")).toString() != QStringLiteral("Pickaxe") ||
            item.value(QStringLiteral("Amount")).toInt() != uniqueAmount)
            continue;
        const QJsonArray components = item.value(QStringLiteral("Components")).toArray();
        if (components.size() != 2 ||
            components.at(0).toObject().value(QStringLiteral("MaterialID")).toString() != QStringLiteral("Iron") ||
            components.at(1).toObject().value(QStringLiteral("MaterialID")).toString() != QStringLiteral("Oak")) {
            qCritical() << "Two-material starting item saved incorrectly";
            Global::cfg = nullptr;
            return 10;
        }
        foundSerialized = true;
    }
    if (!foundSerialized) {
        qCritical() << "Combined starting item missing from saved embark setup";
        Global::cfg = nullptr;
        return 11;
    }

    bool foundReloaded = false;
    {
        NewGameSettings reloaded;
        for (const auto& item : reloaded.startingItems()) {
            if (item.itemSID == QStringLiteral("Pickaxe") && item.amount == uniqueAmount &&
                item.mat1 == QStringLiteral("Iron") && item.mat2 == QStringLiteral("Oak"))
                foundReloaded = true;
        }
    }
    if (!foundReloaded) {
        qCritical() << "Two-material starting item was corrupted on reload";
        Global::cfg = nullptr;
        return 12;
    }
    if (!QDir(QDir(isolatedRoot).filePath(QStringLiteral("save"))).isEmpty()) {
        qCritical() << "NewGameSettings unexpectedly created a colony save";
        Global::cfg = nullptr;
        return 13;
    }

    Global::cfg = nullptr;
    qInfo() << "Original NewGameSettings bootstrap and dual-material roundtrip passed in temp data root.";
    return 0;
}
