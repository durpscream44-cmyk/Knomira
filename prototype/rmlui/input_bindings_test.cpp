// Copyright (C) 2026 Knomira contributors.
// Dependency-free test for remappable controls, conflict checks and persistence.
#include "input_bindings.h"
#include <QCoreApplication>
#include <QTemporaryDir>
#include <QFile>
#include <QDebug>

using KnomiraInput::Profile;

static bool Check(bool value, const char* description) {
    if (!value) qCritical() << "Controls test FAILED:" << description;
    return value;
}

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    Profile original;
    if (!Check(original.keyMatches("menu_accept", Qt::Key_Return),
               "Enter should activate by default")) return 1;
    if (!Check(original.padMatches("menu_accept", QSet<QString>{"A"}),
               "Xbox A should activate by default")) return 2;
    if (!Check(original.keyMatches("camera_up", Qt::Key_W),
               "World camera binding defaults must exist for later integration")) return 3;

    QString reason;
    if (!Check(!original.bindKey(0, 0, Qt::Key_Down, &reason),
               "Conflicting key binding should be rejected")) return 4;
    if (!Check(!original.bindKey(0, 0, Qt::Key_Tab, &reason),
               "Tab recovery navigation must remain reserved")) return 18;
    if (!Check(!original.bindKey(0, 0, int(Qt::CTRL) | Qt::Key_Escape, &reason),
               "Modified Escape cannot be assigned to unreachable menu action")) return 19;
    if (!Check(!original.bindPad(0, 0, "DPadDown", &reason),
               "Conflicting controller binding should be rejected")) return 5;
    if (!Check(original.bindKey(0, 0, Qt::Key_F3, &reason),
               "F3 should be a valid replacement for previous-menu")) return 6;
    if (!Check(original.bindPad(0, 0, "X", &reason),
               "Xbox X should be a valid replacement for previous-menu")) return 7;
    if (!Check(!original.keyMatches("menu_prev", Qt::Key_Up),
               "The replaced primary key must stop triggering")) return 8;
    if (!Check(original.keyMatches("menu_prev", Qt::Key_F3) &&
               original.padMatches("menu_prev", QSet<QString>{"X"}),
               "New bindings must trigger the action")) return 9;

    // The table's always-visible readouts use these exact label helpers.
    // Verify that a changed binding has a readable value independent of the
    // capture dialog and that an empty slot has a distinct placeholder.
    if (!Check(!Profile::keyLabel(original.actions()[0].keys[0]).isEmpty() &&
               Profile::keyLabel(original.actions()[0].keys[0]) != "Unbound",
               "Changed primary keyboard binding must have a visible label")) return 20;
    if (!Check(Profile::padLabel(original.actions()[0].pads[0]) == "Xbox X",
               "Changed primary Xbox binding must have a visible label")) return 21;
    if (!Check(Profile::keyLabel(0) == "Unbound" &&
               Profile::padLabel(QString()) == "Unbound",
               "Unassigned slots must have a visible fallback label")) return 22;

    QTemporaryDir temp;
    if (!Check(temp.isValid(), "temporary config folder")) return 10;
    const QString fileName = temp.path() + "/controls.json";
    if (!Check(original.saveToFile(fileName, &reason), "save profile")) return 11;
    Profile restored;
    if (!Check(restored.loadFromFile(fileName, &reason), "load profile")) return 12;
    if (!Check(restored.keyMatches("menu_prev", Qt::Key_F3) &&
               restored.padMatches("menu_prev", QSet<QString>{"X"}),
               "round-trip keyboard and controller bindings")) return 13;

    restored.resetDefaults();
    if (!Check(restored.keyMatches("menu_prev", Qt::Key_Up) &&
               restored.padMatches("menu_prev", QSet<QString>{"DPadUp"}),
               "reset should restore defaults")) return 14;

    QFile corrupt(fileName);
    if (!Check(corrupt.open(QIODevice::WriteOnly | QIODevice::Truncate), "open test file")) return 15;
    corrupt.write("{broken json");
    corrupt.close();
    if (!Check(!restored.loadFromFile(fileName, &reason),
               "malformed config should not be accepted")) return 16;
    if (!Check(restored.keyMatches("menu_accept", Qt::Key_Return),
               "bad config should leave a safe default menu profile")) return 17;

    qInfo() << "PASS: Remappable keyboard and Xbox profile, conflict detection, JSON persistence and defaults.";
    return 0;
}
