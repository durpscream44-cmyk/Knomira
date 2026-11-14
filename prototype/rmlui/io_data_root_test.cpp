// Copyright (C) 2026 Knomira contributors.
// Standalone test for the EXACT resolver used by original IO::getDataFolder.
// Compiled with KNOMIRA_ISOLATED_TEST_DATA_ROOT; never uses real saves.
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
    const QString selected = Knomira::DataPaths::userDataFolder();
    if (selected != expected)
        return 6;

    const QDir root(selected);
    if (!root.mkpath(QStringLiteral("save")) || !root.mkpath(QStringLiteral("settings")))
        return 7;

    QFile testSave(root.filePath(QStringLiteral("save/smoke-test.txt")));
    if (!testSave.open(QIODevice::WriteOnly) || testSave.write("temporary only\n") <= 0)
        return 8;
    testSave.close();
    if (!QFileInfo::exists(testSave.fileName()))
        return 9;

    // Guard against accepting an unmarked child or path traversal.
    if (!Knomira::DataPaths::validateIsolatedTestRoot(sandbox.path() + QStringLiteral("/save")).isEmpty() ||
        !Knomira::DataPaths::validateIsolatedTestRoot(sandbox.path() + QStringLiteral("/../")).isEmpty())
        return 10;

    qInfo() << "Isolated engine data root passed; test files remain inside temporary sandbox.";
    return 0;
}
