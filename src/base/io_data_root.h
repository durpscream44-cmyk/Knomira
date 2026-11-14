// Copyright (C) 2026 Knomira contributors.
// Isolated test-data path selection for PLAY-01B engine startup tests.
// The shipping game retains Ingnomia's original save directory.
#pragma once

#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include <QString>
#include <QtGlobal>

namespace Knomira::DataPaths {

// A test override is accepted only for an existing, explicitly marked
// directory *beneath* the operating system's temporary directory. Prevent
// accidental writes to Documents/My Games/Ingnomia during headless tests.
// This validation is also usable by the standalone smoke test.
inline QString validateIsolatedTestRoot(const QString& candidate)
{
    if (candidate.isEmpty() || !QDir::isAbsolutePath(candidate))
        return {};

    const QString root = QDir(candidate).canonicalPath();
    const QString temporary = QDir(QDir::tempPath()).canonicalPath();
    if (root.isEmpty() || temporary.isEmpty())
        return {};

#ifdef Q_OS_WIN
    constexpr Qt::CaseSensitivity caseMode = Qt::CaseInsensitive;
#else
    constexpr Qt::CaseSensitivity caseMode = Qt::CaseSensitive;
#endif
    // A directory equal to %TEMP%, or one beside it, is not a sandbox.
    if (!root.startsWith(temporary + QLatin1Char('/'), caseMode))
        return {};
    if (!QFileInfo(root + QStringLiteral("/.knomira-isolated-test-root")).isFile())
        return {};

    return root;
}

inline QString userDataFolder()
{
#ifdef KNOMIRA_ISOLATED_TEST_DATA_ROOT
    // This opt-in macro must only be set for test bootstrap binaries. Both
    // a valid environment variable and a sandbox marker are mandatory.
    // An invalid request must fail closed, never fall back to real saves.
    const QString isolated = validateIsolatedTestRoot(
        qEnvironmentVariable("KNOMIRA_TEST_DATA_ROOT"));
    if (isolated.isEmpty())
        qFatal("KNOMIRA_TEST_DATA_ROOT must be a marked directory beneath the system temp folder");
    return isolated;
#elif defined(Q_OS_WIN)
    return QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
           + QStringLiteral("/My Games/Ingnomia");
#else
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
           + QStringLiteral("/Ingnomia");
#endif
}

} // namespace Knomira::DataPaths
