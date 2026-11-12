// Copyright (C) 2026 Knomira contributors.
// Simple dependency-free QtCore smoke test for the original-save catalog.
#include "save_catalog.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QTemporaryDir>
#include <QTextStream>
#include <QDebug>

namespace {
bool WriteFile(const QString& path, const QByteArray& contents) {
    QFile f(path);
    return f.open(QIODevice::WriteOnly) && f.write(contents) == contents.size();
}

bool Check(bool condition, const char* message) {
    if (!condition) qCritical() << "Save catalog test failed:" << message;
    return condition;
}
}

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);
    QTemporaryDir temp;
    if (!Check(temp.isValid(), "could not create temporary directory"))
        return 1;

    QDir root(temp.path());
    if (!root.mkpath("Kingdom One/autosave") ||
        !root.mkpath("Kingdom One/1") ||
        !root.mkpath("Kingdom One/2.backup") ||
        !root.mkpath("Broken/1"))
        return 2;

    if (!WriteFile(temp.path() + "/Kingdom One/autosave/game.json",
                   R"([{"kingdomName":"Kingdom One","Version":"0.9.0.0"}])") ||
        !WriteFile(temp.path() + "/Kingdom One/1/game.json",
                   R"([{"kingdomName":"Kingdom One","version":"0.8.0.0"}])") ||
        !WriteFile(temp.path() + "/Kingdom One/2.backup/game.json",
                   R"([{"kingdomName":"Ignored Backup"}])") ||
        !WriteFile(temp.path() + "/Broken/1/game.json", "this is not JSON"))
        return 3;

    const auto saves = findIngnomiaSavesInRoot(temp.path());
    if (!Check(saves.size() == 2, "expected exactly two valid saves"))
        return 4;

    bool foundAutosave = false;
    bool foundSlot = false;
    for (const auto& save : saves) {
        if (!Check(save.kingdomName == "Kingdom One", "incorrect kingdom name"))
            return 5;
        if (!Check(QDir(save.absolutePath).exists(), "missing save directory"))
            return 6;
        if (save.slotName == "autosave") {
            foundAutosave = (save.version == "0.9.0.0");
        } else if (save.slotName == "1") {
            foundSlot = (save.version == "0.8.0.0");
        }
    }
    if (!Check(foundAutosave && foundSlot, "save slot/version extraction failed"))
        return 7;

    qInfo() << "PASS: Noesis-free save catalog scans valid original-format saves without modifying them.";
    return 0;
}
