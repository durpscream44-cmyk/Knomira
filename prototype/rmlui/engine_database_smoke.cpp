// Copyright (C) 2026 Knomira contributors.
// Read-only-on-disk bootstrap check using the original Ingnomia SQL asset.
// This deliberately mirrors DB::init()'s semicolon-separated SQL replay,
// but uses an in-memory SQLite connection instead of constructing Config,
// initializing game globals, or writing to player settings/saves.

#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QString>
#include <QStringList>

namespace {
bool RequireTableRows(QSqlDatabase& db, const QString& table) {
    // Keep the allowlist explicit: do not construct SQL from untrusted names.
    QSqlQuery query(db);
    if (!query.exec(QStringLiteral("SELECT COUNT(*) FROM \"") + table +
                    QStringLiteral("\"")) || !query.next()) {
        qCritical() << "Bootstrap DB missing or unreadable table" << table
                    << query.lastError().text();
        return false;
    }
    if (query.value(0).toInt() <= 0) {
        qCritical() << "Bootstrap DB has no data in" << table;
        return false;
    }
    qInfo() << "DB content verified" << table << "rows" << query.value(0).toInt();
    return true;
}
}

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    if (app.arguments().size() != 2) {
        qCritical() << "Usage: knomira_engine_db_bootstrap_test <original ingnomia.db.sql path>";
        return 1;
    }

    const QString source = app.arguments().at(1);
    QFile file(source);
    if (!QFileInfo::exists(source) || !file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical() << "Missing/invalid original game database asset:" << source
                    << file.errorString();
        return 2;
    }
    const QString sql = QString::fromUtf8(file.readAll());
    file.close();
    if (sql.size() < 1000 || !sql.startsWith(QStringLiteral("BEGIN TRANSACTION"))) {
        qCritical() << "Game SQL asset missing or unexpectedly truncated:" << source;
        return 3;
    }

    // Never create a writable SQLite file and never resolve the player's
    // Documents/Ingnomia directory. The source SQL is only read, not changed.
    QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"),
                                                QStringLiteral("knomira_readonly_bootstrap"));
    db.setDatabaseName(QStringLiteral(":memory:"));
    if (!db.open()) {
        qCritical() << "In-memory SQLite open failed:" << db.lastError().text();
        return 4;
    }
    {
        QSqlQuery query(db);
        // The original DB::init uses the same split-and-execute semantics.
        const QStringList statements = sql.split(QLatin1Char(';'));
        int executed = 0;
        for (const QString& statement : statements) {
            const QString command = statement.trimmed();
            if (command.isEmpty())
                continue;
            if (!query.exec(command)) {
                qCritical() << "Original SQL bootstrap failed at statement"
                            << (executed + 1) << query.lastError().text()
                            << "SQL prefix:" << command.left(100);
                return 5;
            }
            ++executed;
        }
        if (executed < 1000) {
            qCritical() << "Unexpectedly small game data SQL:" << executed;
            return 6;
        }
        qInfo() << "Replayed original game data statements:" << executed;
    }

    for (const QString& table : {QStringLiteral("Items"),
                                 QStringLiteral("Materials"),
                                 QStringLiteral("Jobs"),
                                 QStringLiteral("Actions"),
                                 QStringLiteral("Workshops")}) {
        if (!RequireTableRows(db, table))
            return 7;
    }
    qInfo() << "Knomira game data bootstrap passed (in-memory only; no saves or config written).";
    return 0;
}
