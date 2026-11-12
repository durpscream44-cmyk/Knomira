// Copyright (C) 2026 Knomira contributors.
// Read-only catalog of original Ingnomia save games, independent of Noesis.
#pragma once

#include <QDateTime>
#include <QString>
#include <QVector>

struct KnomiraSaveEntry {
    QString kingdomName;
    QString slotName;
    QString absolutePath;
    QString version;
    QDateTime modifiedAt;
};

// Enumerate only immediate children of the original Ingnomia save folder.
// Never modify or rewrite any save; this is an index, not a loader.
QVector<KnomiraSaveEntry> findIngnomiaSaves();

// Windows: %USERPROFILE%/Documents/My Games/Ingnomia/save
// Mirrors IO::getDataFolder() in the existing game code.
QString ingnomiaSaveRoot();
