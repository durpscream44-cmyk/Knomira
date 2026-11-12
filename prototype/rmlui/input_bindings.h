// Copyright (C) 2026 Knomira contributors.
// Shared, UI-independent action bindings for menu and upcoming gameplay.
#pragma once

#include <QSet>
#include <QString>
#include <QStringList>
#include <QVector>

namespace KnomiraInput {

struct Action {
    QString id;
    QString label;
    QString group; // Context: Menu, World, Panels, System. Conflicts are scoped.
    int keys[2] = {0, 0}; // Qt::Key | Qt modifier bits; 0 means unbound.
    QString pads[2];      // Logical XInput token; empty means unbound.
    bool activeInShell = false;
};

class Profile {
public:
    Profile();
    const QVector<Action>& actions() const { return actions_; }
    void resetDefaults();

    bool keyMatches(const QString& actionId, int key) const;
    bool padMatches(const QString& actionId, const QSet<QString>& held) const;

    bool bindKey(int index, int slot, int key, QString* reason = nullptr);
    bool bindPad(int index, int slot, const QString& token, QString* reason = nullptr);

    bool load(QString* error = nullptr);
    bool save(QString* error = nullptr) const;
    bool loadFromFile(const QString& path, QString* error = nullptr);
    bool saveToFile(const QString& path, QString* error = nullptr) const;

    static QString configPath();
    static QString keyLabel(int key);
    static QString padLabel(const QString& token);
    static int normalizeKey(int key, int modifiers);
    static QStringList availablePadTokens();

private:
    int find(const QString& id) const;
    QVector<Action> actions_;
};
} // namespace KnomiraInput
