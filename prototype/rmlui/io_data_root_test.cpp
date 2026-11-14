// Copyright (C) 2026 Knomira contributors.
// Standalone test for the EXACT resolver used by original IO::getDataFolder.
// Compiled in both configurations; calls real IO::getDataFolder, not just helper.
#include "../../src/base/io.h"
#include "../../src/base/io_data_root.h"

#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QTemporaryDir>

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    QTemporaryDir sandbox;
    QTemporaryDir unmarked;
    if (!sandbox.isValid() || !unmarked.isValid())
        return 1;

    // We must never accept an unmarked directory, a relative path, the
    // entire OS temp folder, or a nonexistent directory as a test root.
    if (!Knomira::DataPaths::validateIsolatedTestRoot({}).isEmpty() ||
        !Knomira::DataPaths::validateIsolatedTestRoot(QStringLiteral(".")).isEmpty() ||
        !Knomira::DataPaths::validateIsolatedTestRoot(QDir::tempPath()).isEmpty() ||
        !Knomira::DataPaths::validateIsolatedTestRoot(unmarked.path()).isEmpty() ||
        !Knomira::DataPaths::validateIsolatedTestRoot(sandbox.path() + QStringLiteral("/absent")).isEmpty()) {
        qCritical() << "Unsafe data-root override incorrectly accepted";
        return 2;
    }

    QFile marker(sandbox.filePath(QStringLiteral(".knomira-isolated-test-root")));
    if (!marker.open(QIODevice::WriteOnly) || marker.write("Knomira isolated test\n") <= 0)
        return 3;
    marker.close();

    const QString expected = QDir(sandbox.path()).canonicalPath();
    const QString accepted = Knomira::DataPaths::validateIsolatedTestRoot(sandbox.path());
    if (accepted != expected)
        return 4;

    // The production resolver is used by IO::getDataFolder. In a test-only
    // binary it must obey the explicit sandbox, not Documents/My Games.
    if (!qputenv("KNOMIRA_TEST_DATA_ROOT", sandbox.path().toLocal8Bit()))
        return 5;
#ifdef KNOMIRA_ISOLATED_TEST_DATA_ROOT
    const QString selected = IO::getDataFolder();
    if (selected != expected)
        return 6;

    const QDir root(selected);
    // Test real, original IO startup path under the isolated resolver.
    // It must never create any of these paths in the player's Documents.
    if (!IO::createFolders())
        return 7;
    for (const QString& directory : {QStringLiteral("mods"), QStringLiteral("save"),
                                     QStringLiteral("screenshots"), QStringLiteral("settings"),
                                     QStringLiteral("tmp")}) {
        if (!root.exists(directory)) {
            qCritical() << "Missing original startup folder" << directory;
            return 8;
        }
    }
    for (const QString& file : {QStringLiteral("config.json"),
                                QStringLiteral("newgame.json"),
                                QStringLiteral("profs.json")}) {
        if (!QFileInfo::exists(root.filePath(QStringLiteral("settings/") + file))) {
            qCritical() << "Missing original packaged startup default" << file;
            return 9;
        }
    }

    // Re-running creation must preserve a user's existing settings.
    const QString settingsPath = root.filePath(QStringLiteral("settings/config.json"));
    QFile existing(settingsPath);
    if (!existing.open(QIODevice::WriteOnly | QIODevice::Truncate) ||
        existing.write("test sandbox sentinel") <= 0)
        return 10;
    existing.close();
    if (!IO::createFolders())
        return 11;
    if (!existing.open(QIODevice::ReadOnly) ||
        existing.readAll() != QByteArray("test sandbox sentinel"))
        return 12;
    existing.close();

    QFile testSave(root.filePath(QStringLiteral("save/smoke-test.txt")));
    if (!testSave.open(QIODevice::WriteOnly) || testSave.write("temporary only\\n") <= 0)
        return 13;
    testSave.close();
    if (!QFileInfo::exists(testSave.fileName()))
        return 14;

    // Guard against accepting an unmarked child or path traversal.
    if (!Knomira::DataPaths::validateIsolatedTestRoot(sandbox.path() + QStringLiteral("/save")).isEmpty() ||
        !Knomira::DataPaths::validateIsolatedTestRoot(sandbox.path() + QStringLiteral("/../")).isEmpty())
        return 15;

    qInfo() << "Real IO folder bootstrap passed and remained within temporary sandbox.";
#else
    // Shipping binaries must ignore the test-only environment variable,
    // even when it points to a perfectly valid marked sandbox.
    const QString selected = IO::getDataFolder();
#ifdef Q_OS_WIN
    const QString standard = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
                           + QStringLiteral("/My Games/Ingnomia");
#else
    const QString standard = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
                           + QStringLiteral("/Ingnomia");
#endif
    if (selected != standard || selected == expected) {
        qCritical() << "Production data root was redirected unexpectedly";
        return 11;
    }
    qInfo() << "Production data-root behavior preserved; test override ignored.";
#endif
    return 0;
}
