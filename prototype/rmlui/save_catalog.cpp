// Copyright (C) 2026 Knomira contributors.
#include "save_catalog.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QStandardPaths>

#include <algorithm>

QString ingnomiaSaveRoot() {
#ifdef Q_OS_WIN
    return QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
        + QStringLiteral("/My Games/Ingnomia/save");
#else
    // The legacy non-Windows IO::getDataFolder() uses AppDataLocation/Ingnomia.
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
        + QStringLiteral("/Ingnomia/save");
#endif
}

QVector<KnomiraSaveEntry> findIngnomiaSaves() {
    QVector<KnomiraSaveEntry> saves;
    const QDir root(ingnomiaSaveRoot());
    if (!root.exists())
        return saves;

    // Ingnomia stores <save>/<kingdom>/<slot>/game.json. Do not recurse into
    // arbitrary user folders or follow symlinks.
    const auto kingdomDirs = root.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    for (const QFileInfo& kingdom : kingdomDirs) {
        const QDir kingdomFolder(kingdom.absoluteFilePath());
        const auto slots = kingdomFolder.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
        for (const QFileInfo& slot : slots) {
            if (slot.fileName().endsWith(QStringLiteral(".backup"), Qt::CaseInsensitive))
                continue;
            const QString gameJson = slot.absoluteFilePath() + QStringLiteral("/game.json");
            QFile input(gameJson);
            if (!input.open(QIODevice::ReadOnly))
                continue;
            // This is metadata, not a full-game parse. Cap memory for corrupt
            // files; a legitimate game.json should be much smaller than 2 MiB.
            if (input.size() > 2 * 1024 * 1024)
                continue;
            QJsonParseError error;
            const QJsonDocument doc = QJsonDocument::fromJson(input.readAll(), &error);
            if (error.error != QJsonParseError::NoError || !doc.isArray() || doc.array().isEmpty())
                continue;

            const QJsonObject metadata = doc.array().first().toObject();
            if (metadata.isEmpty())
                continue;

            KnomiraSaveEntry entry;
            entry.kingdomName = metadata.value(QStringLiteral("kingdomName")).toString(kingdom.fileName());
            entry.slotName = slot.fileName();
            entry.absolutePath = slot.absoluteFilePath();
            // Legacy saves have used both "Version" and "version" fields.
            entry.version = metadata.value(QStringLiteral("Version")).toString();
            if (entry.version.isEmpty())
                entry.version = metadata.value(QStringLiteral("version")).toString(QStringLiteral("unknown"));
            entry.modifiedAt = QFileInfo(gameJson).lastModified();
            saves.push_back(std::move(entry));
        }
    }

    std::sort(saves.begin(), saves.end(), [](const KnomiraSaveEntry& a, const KnomiraSaveEntry& b) {
        return a.modifiedAt > b.modifiedAt;
    });
    return saves;
}
