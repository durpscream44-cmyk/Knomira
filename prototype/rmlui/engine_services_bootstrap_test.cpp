// Copyright (C) 2026 Knomira contributors.
// PLAY-01B2: initialize real Config, DB, workshop/job caches and Strings
// without linking a fake GameManager or running a world. All filesystem
// writes by real Config/IO are confined to a marked QTemporaryDir.
#include "../../src/base/config.h"
#include "../../src/base/db.h"
#include "../../src/base/global.h"
#include "../../src/base/io.h"
#include "../../src/base/io_data_root.h"
#include "../../src/gui/strings.h"
#include "../../src/game/newgamesettings.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTemporaryDir>

// Minimal static game-global symbols used by actual DB/Strings/Position
// methods when deliberately linking without the complete GameManager.
// These are NOT substitute database, configuration, or simulation objects.
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
        marker.write("Knomira real engine DB bootstrap test\n") <= 0)
        return 2;
    marker.close();

    const QString expected = QDir(sandbox.path()).canonicalPath();
    if (!qputenv("KNOMIRA_TEST_DATA_ROOT", expected.toLocal8Bit()) ||
        Knomira::DataPaths::validateIsolatedTestRoot(expected) != expected ||
        IO::getDataFolder() != expected) {
        qCritical() << "Refusing engine DB startup outside isolated directory";
        return 3;
    }

    Config config;
    if (!config.valid()) {
        qCritical() << "Real configuration initialization failed";
        return 4;
    }
    Global::cfg = &config;
    if (!QFileInfo::exists(config.get(QStringLiteral("dataPath")).toString() +
                           QStringLiteral("/db/ingnomia.db.sql"))) {
        qCritical() << "The original game SQL asset is not packaged";
        Global::cfg = nullptr;
        return 5;
    }

    DB::init();        // Actual game's SQL replay, no standalone fixture.
    const int itemCount = DB::numRows(QStringLiteral("Items"));
    const int jobCount = DB::numRows(QStringLiteral("Jobs"));
    const int translationCount = DB::numRows(QStringLiteral("Translation"));
    if (itemCount <= 0 || jobCount <= 0 || translationCount <= 0) {
        qCritical() << "Original DB::init did not populate game data"
                    << itemCount << jobCount << translationCount;
        Global::cfg = nullptr;
        return 6;
    }

    // Actual game startup initializes DB caches after importing base SQL.
    DB::initStructs();
    const QList<QString> jobIds = DB::jobIds();
    if (jobIds.isEmpty() || DB::job(jobIds.first()).isNull()) {
        qCritical() << "Original job struct cache failed to initialize";
        Global::cfg = nullptr;
        return 7;
    }
    const auto workshops = DB::selectRows(QStringLiteral("Workshops"));
    if (workshops.isEmpty() ||
        DB::workshop(workshops.first().value(QStringLiteral("ID")).toString()).isNull()) {
        qCritical() << "Original workshop struct cache failed to initialize";
        Global::cfg = nullptr;
        return 8;
    }

    if (!Strings::gi().init()) {
        qCritical() << "Original game localization initialization failed";
        Global::cfg = nullptr;
        return 9;
    }
    const auto translations = DB::selectRows(QStringLiteral("Translation"));
    if (translations.isEmpty()) {
        Global::cfg = nullptr;
        return 10;
    }
    const QString id = translations.first().value(QStringLiteral("ID")).toString();
    const QString expectedText = translations.first().value(QStringLiteral("Text")).toString();
    if (id.isEmpty() || Strings::s(id) != expectedText) {
        qCritical() << "Original Strings lookup did not match actual DB row" << id;
        Global::cfg = nullptr;
        return 11;
    }

    // Real NewGameSettings constructor exercises original embark settings,
    // species filters, localized names, presets and random seed generation.
    NewGameSettings embark;
    if (embark.worldSize() <= 0 || embark.zLevels() <= 0 ||
        embark.numGnomes() <= 0 || embark.seed().isEmpty() ||
        embark.kingdomName().isEmpty() || embark.startingItems().isEmpty()) {
        qCritical() << "Original NewGameSettings failed to initialize from game content";
        Global::cfg = nullptr;
        return 13;
    }

    // The shipped starting tools use identical material IDs for both
    // components, concealing a real bug in setStartingItems(). Vary the
    // material on the second component IN THE TEST SANDBOX ONLY.
    const QString newGameFile = QDir(expected).filePath(
        QStringLiteral("settings/newgame.json"));
    QJsonDocument embarkDoc;
    if (!IO::loadFile(newGameFile, embarkDoc) || !embarkDoc.isObject()) {
        qCritical() << "Missing original new-game JSON defaults in sandbox";
        Global::cfg = nullptr;
        return 14;
    }
    QJsonObject root = embarkDoc.object();
    QJsonArray starting = root.value(QStringLiteral("startingItems")).toArray();
    QString twoPartItem;
    QString firstMaterial;
    QString secondMaterial;
    for (qsizetype i = 0; i < starting.size(); ++i) {
        QJsonObject item = starting.at(i).toObject();
        if (item.value(QStringLiteral("Type")).toString() != QStringLiteral("CombinedItem"))
            continue;
        QJsonArray components = item.value(QStringLiteral("Components")).toArray();
        if (components.size() != 2)
            continue;
        QJsonObject first = components.at(0).toObject();
        QJsonObject second = components.at(1).toObject();
        firstMaterial = first.value(QStringLiteral("MaterialID")).toString();
        secondMaterial = firstMaterial == QStringLiteral("Iron")
                             ? QStringLiteral("Pine") : QStringLiteral("Iron");
        if (firstMaterial.isEmpty() || firstMaterial == secondMaterial)
            continue;
        second.insert(QStringLiteral("MaterialID"), secondMaterial);
        components.replace(1, second);
        item.insert(QStringLiteral("Components"), components);
        starting.replace(i, item);
        twoPartItem = item.value(QStringLiteral("ItemID")).toString();
        break;
    }
    if (twoPartItem.isEmpty()) {
        qCritical() << "No combined starting item in original embark defaults";
        Global::cfg = nullptr;
        return 15;
    }
    root.insert(QStringLiteral("startingItems"), starting);
    if (!IO::saveFile(newGameFile, QJsonDocument(root))) {
        qCritical() << "Failed to write test-only modified embark defaults";
        Global::cfg = nullptr;
        return 16;
    }
    NewGameSettings parsed;
    bool componentsCorrect = false;
    for (const StartingItem& item : parsed.startingItems()) {
        if (item.itemSID == twoPartItem && item.mat1 == firstMaterial &&
            item.mat2 == secondMaterial) {
            componentsCorrect = true;
            break;
        }
    }
    if (!componentsCorrect) {
        qCritical() << "Two-component starting-item material mapping is incorrect"
                    << twoPartItem << firstMaterial << secondMaterial;
        Global::cfg = nullptr;
        return 17;
    }

    // Config/DB reads must not create a test save; the sandbox is disposable.
    if (!QDir(QDir(expected).filePath(QStringLiteral("save"))).isEmpty()) {
        qCritical() << "Engine service bootstrap unexpectedly created a save";
        Global::cfg = nullptr;
        return 12;
    }
    Global::cfg = nullptr;
    qInfo() << "Original Config/DB/Strings/NewGameSettings initialization passed"
            << "items" << itemCount << "jobs" << jobCount
            << "translations" << translationCount;
    return 0;
}
