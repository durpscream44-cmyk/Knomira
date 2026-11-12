// Copyright (C) 2026 Knomira contributors.
#include "input_bindings.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QKeySequence>
#include <QSaveFile>
#include <QStandardPaths>

#include <utility>

namespace KnomiraInput {
namespace {

const QStringList kPadTokens = {
    "DPadUp", "DPadDown", "DPadLeft", "DPadRight",
    "A", "B", "X", "Y", "LB", "RB", "Back", "Start",
    "LThumb", "RThumb", "LSUp", "LSDown", "LSLeft", "LSRight",
    "RSUp", "RSDown", "RSLeft", "RSRight", "LT", "RT"
};

QString padPretty(const QString& token) {
    if (token.startsWith("DPad")) return "D-pad " + token.mid(4);
    if (token.startsWith("LS")) return "Left stick " + token.mid(2);
    if (token.startsWith("RS")) return "Right stick " + token.mid(2);
    if (token == "LB" || token == "RB" || token == "LT" || token == "RT")
        return "Xbox " + token;
    if (token == "LThumb") return "Left stick click";
    if (token == "RThumb") return "Right stick click";
    return "Xbox " + token;
}

bool validate(const QVector<Action>& actions, QString* error) {
    for (const auto& action : actions) {
        // Escape is always an emergency route back to the menu, even if the
        // user's configured Back binding changes. Don't offer a dead mapping.
        if (action.group == "Menu" && action.id != "menu_back" &&
            (action.keys[0] == Qt::Key_Escape || action.keys[1] == Qt::Key_Escape)) {
            if (error) *error = "Escape is reserved as the menu recovery shortcut.";
            return false;
        }
        for (int slot = 0; slot < 2; ++slot) {
            const auto& token = action.pads[slot];
            if (!token.isEmpty() && !kPadTokens.contains(token)) {
                if (error) *error = "Unknown controller input " + token + " for " + action.label;
                return false;
            }
        }
        if (action.activeInShell && action.keys[0] == 0 && action.keys[1] == 0) {
            if (error) *error = "Keep at least one keyboard binding for " + action.label;
            return false;
        }
        if (action.activeInShell && action.pads[0].isEmpty() && action.pads[1].isEmpty()) {
            if (error) *error = "Keep at least one gamepad binding for " + action.label;
            return false;
        }
    }
    for (int i = 0; i < actions.size(); ++i) {
        for (int j = i + 1; j < actions.size(); ++j) {
            if (actions[i].group != actions[j].group) continue;
            for (int a = 0; a < 2; ++a) {
                for (int b = 0; b < 2; ++b) {
                    if (actions[i].keys[a] != 0 && actions[i].keys[a] == actions[j].keys[b]) {
                        if (error) *error = KnomiraInput::Profile::keyLabel(actions[i].keys[a]) + " already belongs to " + actions[j].label; // placeholder replaced below
                        return false;
                    }
                    if (!actions[i].pads[a].isEmpty() && actions[i].pads[a] == actions[j].pads[b]) {
                        if (error) *error = actions[i].pads[a] + " already belongs to " + actions[j].label;
                        return false;
                    }
                }
            }
        }
    }
    return true;
}

} // namespace

Profile::Profile() { resetDefaults(); }

void Profile::resetDefaults() {
    actions_.clear();
    auto add = [this](const char* id, const char* label, const char* group,
                      int keyA, int keyB, const char* padA, const char* padB,
                      bool live = false) {
        Action a;
        a.id = QString::fromUtf8(id);
        a.label = QString::fromUtf8(label);
        a.group = QString::fromUtf8(group);
        a.keys[0] = keyA; a.keys[1] = keyB;
        a.pads[0] = QString::fromUtf8(padA);
        a.pads[1] = QString::fromUtf8(padB);
        a.activeInShell = live;
        actions_.push_back(std::move(a));
    };
    // Fully active, with independently changeable primary and secondary inputs.
    add("menu_prev", "Previous menu item", "Menu", Qt::Key_Up, Qt::Key_Left, "DPadUp", "LSUp", true);
    add("menu_next", "Next menu item", "Menu", Qt::Key_Down, Qt::Key_Right, "DPadDown", "LSDown", true);
    add("menu_accept", "Activate / select", "Menu", Qt::Key_Return, Qt::Key_Space, "A", "Start", true);
    add("menu_back", "Back / cancel", "Menu", Qt::Key_Escape, Qt::Key_Backspace, "B", "Back", true);

    // Stored now, dispatched once the world simulation/input router is attached.
    add("camera_up", "Camera up", "World (planned)", Qt::Key_W, 0, "LSUp", "");
    add("camera_down", "Camera down", "World (planned)", Qt::Key_S, 0, "LSDown", "");
    add("camera_left", "Camera left", "World (planned)", Qt::Key_A, 0, "LSLeft", "");
    add("camera_right", "Camera right", "World (planned)", Qt::Key_D, 0, "LSRight", "");
    add("zoom_in", "Zoom in", "World (planned)", Qt::Key_Plus, Qt::Key_Equal, "RT", "");
    add("zoom_out", "Zoom out", "World (planned)", Qt::Key_Minus, 0, "LT", "");
    add("level_up", "Level up", "World (planned)", Qt::Key_PageUp, 0, "Y", "");
    add("level_down", "Level down", "World (planned)", Qt::Key_PageDown, 0, "X", "");
    add("rotate_left", "Rotate left", "World (planned)", Qt::Key_Q, 0, "DPadLeft", "");
    add("rotate_right", "Rotate right", "World (planned)", Qt::Key_E, 0, "DPadRight", "");
    add("pause", "Pause / resume", "World (planned)", Qt::Key_Space, 0, "Start", "");
    add("speed_up", "Increase game speed", "World (planned)", Qt::Key_Period, 0, "RB", "");
    add("speed_down", "Decrease game speed", "World (planned)", Qt::Key_Comma, 0, "LB", "");
    add("select_tile", "Select tile / confirm tool", "World (planned)", 0, 0, "A", "");
    add("cancel_tool", "Cancel current tool", "World (planned)", Qt::Key_Escape, 0, "B", "");
    add("build", "Open build menu", "Panels (planned)", Qt::Key_B, 0, "", "");
    add("inventory", "Open inventory", "Panels (planned)", Qt::Key_I, 0, "", "");
    add("stockpiles", "Open stockpiles", "Panels (planned)", Qt::Key_K, 0, "", "");
    add("workshops", "Open workshops", "Panels (planned)", Qt::Key_T, 0, "", "");
    add("agriculture", "Open farming", "Panels (planned)", Qt::Key_F, 0, "", "");
    add("save_game", "Save game", "System (planned)", int(Qt::CTRL) | Qt::Key_S, 0, "", "");
    add("load_game", "Load game", "System (planned)", int(Qt::CTRL) | Qt::Key_L, 0, "", "");
}

int Profile::find(const QString& id) const {
    for (int i = 0; i < actions_.size(); ++i)
        if (actions_[i].id == id) return i;
    return -1;
}

bool Profile::keyMatches(const QString& id, int code) const {
    const int i = find(id);
    return i >= 0 && code != 0 && (actions_[i].keys[0] == code || actions_[i].keys[1] == code);
}

bool Profile::padMatches(const QString& id, const QSet<QString>& held) const {
    const int i = find(id);
    if (i < 0) return false;
    return (!actions_[i].pads[0].isEmpty() && held.contains(actions_[i].pads[0])) ||
           (!actions_[i].pads[1].isEmpty() && held.contains(actions_[i].pads[1]));
}

bool Profile::bindKey(int index, int slot, int key, QString* reason) {
    if (index < 0 || index >= actions_.size() || slot < 0 || slot > 1) return false;
    auto draft = actions_;
    draft[index].keys[slot] = key;
    if (draft[index].keys[0] == draft[index].keys[1] && key != 0) {
        if (reason) *reason = "Already assigned to the other keyboard slot.";
        return false;
    }
    if (!validate(draft, reason)) return false;
    actions_ = std::move(draft);
    return true;
}

bool Profile::bindPad(int index, int slot, const QString& token, QString* reason) {
    if (index < 0 || index >= actions_.size() || slot < 0 || slot > 1) return false;
    auto draft = actions_;
    draft[index].pads[slot] = token;
    if (draft[index].pads[0] == draft[index].pads[1] && !token.isEmpty()) {
        if (reason) *reason = "Already assigned to the other controller slot.";
        return false;
    }
    if (!validate(draft, reason)) return false;
    actions_ = std::move(draft);
    return true;
}

QString Profile::configPath() {
    return QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation)
        + QStringLiteral("/controls.json");
}

bool Profile::save(QString* error) const { return saveToFile(configPath(), error); }
bool Profile::load(QString* error) { return loadFromFile(configPath(), error); }

bool Profile::saveToFile(const QString& path, QString* error) const {
    if (!validate(actions_, error)) return false;
    if (!QDir().mkpath(QFileInfo(path).absolutePath())) {
        if (error) *error = "Cannot create controls configuration directory.";
        return false;
    }
    QJsonObject entries;
    for (const auto& item : actions_) {
        QJsonObject row;
        row["keys"] = QJsonArray{item.keys[0], item.keys[1]};
        row["gamepad"] = QJsonArray{item.pads[0], item.pads[1]};
        entries[item.id] = row;
    }
    const QJsonObject body{{"version", 1}, {"actions", entries}};
    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        if (error) *error = file.errorString();
        return false;
    }
    const auto bytes = QJsonDocument(body).toJson(QJsonDocument::Indented);
    if (file.write(bytes) != bytes.size() || !file.commit()) {
        if (error) *error = file.errorString();
        return false;
    }
    return true;
}

bool Profile::loadFromFile(const QString& path, QString* error) {
    resetDefaults();
    QFile file(path);
    if (!file.exists()) return true;
    if (!file.open(QIODevice::ReadOnly)) {
        if (error) *error = file.errorString();
        return false;
    }
    if (file.size() > 256 * 1024) {
        if (error) *error = "Controls file exceeds the permitted size.";
        return false;
    }
    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        if (error) *error = "Invalid JSON in controls file.";
        return false;
    }
    const QJsonObject data = document.object();
    if (data.value("version").toInt() != 1 || !data.value("actions").isObject()) {
        if (error) *error = "Unsupported controls file format.";
        return false;
    }
    auto draft = actions_;
    const auto rows = data.value("actions").toObject();
    for (auto& item : draft) {
        if (!rows.value(item.id).isObject()) continue;
        const QJsonObject row = rows.value(item.id).toObject();
        const auto keys = row.value("keys").toArray();
        const auto pads = row.value("gamepad").toArray();
        if (keys.size() != 2 || pads.size() != 2) continue;
        if (!keys[0].isDouble() || !keys[1].isDouble() ||
            !pads[0].isString() || !pads[1].isString()) continue;
        item.keys[0] = keys[0].toInt();
        item.keys[1] = keys[1].toInt();
        item.pads[0] = pads[0].toString();
        item.pads[1] = pads[1].toString();
    }
    if (!validate(draft, error)) return false;
    actions_ = std::move(draft);
    return true;
}

QString Profile::keyLabel(int key) {
    return key ? QKeySequence(key).toString(QKeySequence::NativeText) : QStringLiteral("Unbound");
}
QString Profile::padLabel(const QString& token) {
    return token.isEmpty() ? QStringLiteral("Unbound") : padPretty(token);
}
QStringList Profile::availablePadTokens() { return kPadTokens; }

int Profile::normalizeKey(int key, int modifiers) {
    if (key == Qt::Key_Enter) key = Qt::Key_Return;
    if (key == Qt::Key_Backtab) { key = Qt::Key_Tab; modifiers |= Qt::SHIFT; }
    constexpr int allowed = Qt::SHIFT | Qt::CTRL | Qt::ALT | Qt::META;
    return key | (modifiers & allowed);
}
} // namespace KnomiraInput
