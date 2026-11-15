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

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
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

    // Regression: DB::initStructs must not attach ALL Jobs_Tasks and
    // Jobs_SpriteID rows to every cached job. Both tables have an ID column
    // naming the owner job. This previously multiplied cached tasks and
    // sprite records and could make unrelated job actions execute.
    const auto rawJobs = DB::selectRows(QStringLiteral("Jobs"));
    qint64 cachedTaskCount = 0;
    qint64 cachedSpriteCount = 0;
    for (const auto& rawJob : rawJobs) {
        const QString jobId = rawJob.value(QStringLiteral("ID")).toString();
        const auto cachedJob = DB::job(jobId);
        if (cachedJob.isNull()) {
            qCritical() << "Missing cached job" << jobId;
            Global::cfg = nullptr;
            return 13;
        }
        const int expectedTasks = DB::selectRows(QStringLiteral("Jobs_Tasks"), jobId).size();
        const int expectedSprites = DB::selectRows(QStringLiteral("Jobs_SpriteID"), jobId).size();
        if (cachedJob->tasks.size() != expectedTasks ||
            cachedJob->sprites.size() != expectedSprites) {
            qCritical() << "Job cache includes another job's tasks/sprites"
                        << jobId << "tasks" << cachedJob->tasks.size() << expectedTasks
                        << "sprites" << cachedJob->sprites.size() << expectedSprites;
            Global::cfg = nullptr;
            return 14;
        }
        cachedTaskCount += cachedJob->tasks.size();
        cachedSpriteCount += cachedJob->sprites.size();
    }
    const qint64 actualTaskCount = DB::numRows(QStringLiteral("Jobs_Tasks"));
    const qint64 actualSpriteCount = DB::numRows(QStringLiteral("Jobs_SpriteID"));
    if (cachedTaskCount != actualTaskCount ||
        cachedSpriteCount != actualSpriteCount) {
        qCritical() << "Job cache task/sprite totals differ from raw DB"
                    << cachedTaskCount << actualTaskCount
                    << cachedSpriteCount << actualSpriteCount;
        Global::cfg = nullptr;
        return 15;
    }
    qInfo() << "Verified per-job task/sprite scope across" << rawJobs.size()
            << "jobs:" << cachedTaskCount << "tasks,"
            << cachedSpriteCount << "sprites";

    // Config/DB reads must not create a test save; the sandbox is disposable.
    if (!QDir(QDir(expected).filePath(QStringLiteral("save"))).isEmpty()) {
        qCritical() << "Engine service bootstrap unexpectedly created a save";
        Global::cfg = nullptr;
        return 12;
    }
    Global::cfg = nullptr;
    qInfo() << "Original Config/DB::init/DB::initStructs/Strings::init passed"
            << "items" << itemCount << "jobs" << jobCount
            << "translations" << translationCount;
    return 0;
}
