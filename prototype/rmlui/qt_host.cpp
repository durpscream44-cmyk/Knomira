// Copyright (C) 2026 Knomira contributors.
// Qt/OpenGL-hosted RmlUi proof of concept. No game state or Noesis dependency.
// The final game adapter should reuse the existing MainWindow QOpenGLContext.
#include <RmlUi/Core.h>
#include <RmlUi/Core/Context.h>
#include <RmlUi/Core/ElementDocument.h>
#include <RmlUi/Core/EventListener.h>
#include <RmlUi/Core/SystemInterface.h>
#include <RmlUi_Renderer_GL3.h>

#include <QClipboard>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QElapsedTimer>
#include <QEvent>
#include <QExposeEvent>
#include <QGuiApplication>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QOpenGLContext>
#include <QResizeEvent>
#include <QSurfaceFormat>
#include <QTimer>
#include <QWheelEvent>
#include <QWindow>
#ifdef KNOMIRA_GAME_SHELL
#include "save_catalog.h"
#include "input_bindings.h"
#include <QSet>
#include <vector>
#include <algorithm>
#endif

#ifdef _WIN32
// Windows SDK XInput.h depends on Windows architecture definitions and must
// be included *after* Windows.h (otherwise winnt.h emits "No Target Architecture").
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <Xinput.h>
#endif

#include <cmath>
#include <array>
#include <memory>
#include <string>

namespace {

class QtSystemInterface final : public Rml::SystemInterface {
public:
    QtSystemInterface() { clock_.start(); }
    double GetElapsedTime() override { return clock_.elapsed() / 1000.0; }
    void SetClipboardText(const Rml::String& text) override {
        QGuiApplication::clipboard()->setText(QString::fromUtf8(text.data(), int(text.size())));
    }
    void GetClipboardText(Rml::String& text) override {
        const QByteArray utf8 = QGuiApplication::clipboard()->text().toUtf8();
        text.assign(utf8.constData(), size_t(utf8.size()));
    }
private:
    QElapsedTimer clock_;
};

int Modifiers(Qt::KeyboardModifiers modifiers) {
    int result = 0;
    if (modifiers & Qt::ShiftModifier) result |= Rml::Input::KM_SHIFT;
    if (modifiers & Qt::ControlModifier) result |= Rml::Input::KM_CTRL;
    if (modifiers & Qt::AltModifier) result |= Rml::Input::KM_ALT;
    if (modifiers & Qt::MetaModifier) result |= Rml::Input::KM_META;
    return result;
}

Rml::Input::KeyIdentifier Key(int qtKey) {
    using namespace Rml::Input;
    if (qtKey >= Qt::Key_A && qtKey <= Qt::Key_Z)
        return static_cast<KeyIdentifier>(KI_A + qtKey - Qt::Key_A);
    if (qtKey >= Qt::Key_0 && qtKey <= Qt::Key_9)
        return static_cast<KeyIdentifier>(KI_0 + qtKey - Qt::Key_0);
    switch (qtKey) {
    case Qt::Key_Escape: return KI_ESCAPE;
    case Qt::Key_Return: return KI_RETURN;
    case Qt::Key_Enter: return KI_NUMPADENTER;
    case Qt::Key_Space: return KI_SPACE;
    case Qt::Key_Tab: return KI_TAB;
    case Qt::Key_Backspace: return KI_BACK;
    case Qt::Key_Delete: return KI_DELETE;
    case Qt::Key_Insert: return KI_INSERT;
    case Qt::Key_Left: return KI_LEFT;
    case Qt::Key_Right: return KI_RIGHT;
    case Qt::Key_Up: return KI_UP;
    case Qt::Key_Down: return KI_DOWN;
    case Qt::Key_Home: return KI_HOME;
    case Qt::Key_End: return KI_END;
    case Qt::Key_PageUp: return KI_PRIOR;
    case Qt::Key_PageDown: return KI_NEXT;
    case Qt::Key_Shift: return KI_LSHIFT;
    case Qt::Key_Control: return KI_LCONTROL;
    case Qt::Key_Alt: return KI_LMENU;
    case Qt::Key_F1: return KI_F1;
    case Qt::Key_F2: return KI_F2;
    case Qt::Key_F3: return KI_F3;
    case Qt::Key_F4: return KI_F4;
    case Qt::Key_F5: return KI_F5;
    case Qt::Key_F6: return KI_F6;
    case Qt::Key_F7: return KI_F7;
    case Qt::Key_F8: return KI_F8;
    case Qt::Key_F9: return KI_F9;
    case Qt::Key_F10: return KI_F10;
    case Qt::Key_F11: return KI_F11;
    case Qt::Key_F12: return KI_F12;
    default: return KI_UNKNOWN;
    }
}

int MouseButton(Qt::MouseButton button) {
    switch (button) {
    case Qt::LeftButton: return 0;
    case Qt::RightButton: return 1;
    case Qt::MiddleButton: return 2;
    default: return -1;
    }
}

bool LoadFont() {
#ifdef _WIN32
    const char* candidates[] = {
        "C:/Windows/Fonts/segoeui.ttf",
        "C:/Windows/Fonts/arial.ttf"
    };
#else
    const char* candidates[] = {
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/liberation2/LiberationSans-Regular.ttf"
    };
#endif
    for (const char* path : candidates) {
        if (Rml::LoadFontFace(path))
            return true;
    }
    return false;
}

constexpr std::array<const char*, 4> MenuButtonIds{"new_game", "load_game", "settings", "quit"};

// Load the Windows XInput system DLL at runtime instead of requiring an SDK
// import library on CI machines. Windows 10/11 provide xinput1_4.dll; older
// supported builds may provide xinput9_1_0.dll.
#ifdef _WIN32
DWORD ReadControllerState(DWORD controller, XINPUT_STATE* state) {
    using GetState = DWORD (WINAPI*)(DWORD, XINPUT_STATE*);
    static const GetState getState = []() -> GetState {
        HMODULE dll = LoadLibraryW(L"xinput1_4.dll");
        if (!dll)
            dll = LoadLibraryW(L"xinput9_1_0.dll");
        if (!dll)
            return nullptr;
        return reinterpret_cast<GetState>(GetProcAddress(dll, "XInputGetState"));
    }();
    return getState ? getState(controller, state) : ERROR_DEVICE_NOT_CONNECTED;
}
#endif

class QtMenuWindow;

class MenuEvents final : public Rml::EventListener {
public:
    explicit MenuEvents(QtMenuWindow* window) : window_(window) {}
    void ProcessEvent(Rml::Event& event) override;
private:
    QtMenuWindow* window_ = nullptr;
};

// This shape deliberately matches the real game's QWindow + QOpenGLContext host.
// No GLFW window is constructed; RmlUi draws directly to the Qt swap chain.
class QtMenuWindow final : public QWindow {
public:
    QtMenuWindow() : events_(this) {
        QSurfaceFormat format;
        format.setRenderableType(QSurfaceFormat::OpenGL);
        format.setProfile(QSurfaceFormat::CoreProfile);
        format.setVersion(4, 3); // Same context profile requested by game MainWindow.
        format.setDepthBufferSize(24);
        format.setStencilBufferSize(8);
        format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
        format.setSwapInterval(1);
        setSurfaceType(QSurface::OpenGLSurface);
        setFormat(format);
#ifdef KNOMIRA_GAME_SHELL
        setTitle(QStringLiteral("Knomira | Noesis-Free Shell"));
#else
        setTitle(QStringLiteral("Knomira | Qt + RmlUi Host"));
#endif
        resize(1280, 800);

#ifdef KNOMIRA_GAME_SHELL
        QString bindingsError;
        if (!controls_.load(&bindingsError))
            qWarning() << "Controls profile invalid; safe defaults used:" << bindingsError;
#endif
        pollClock_.start();
        QObject::connect(&timer_, &QTimer::timeout, this, [this] {
            PollGamepad();
            if (isExposed()) requestUpdate();
        });
        timer_.setInterval(16); // UI demonstration only; NOT simulation ticks.
        timer_.start();
    }

    ~QtMenuWindow() override {
        timer_.stop();
        if (glContext_ && glContext_->makeCurrent(this)) {
            if (rmlInitialised_) Rml::Shutdown();
            renderer_.reset();
            if (glLoaderInitialised_) RmlGL3::Shutdown();
            glContext_->doneCurrent();
        }
    }

    void SetStatus(const char* message) {
        if (document_) {
            if (Rml::Element* status = document_->GetElementById("status"))
                status->SetInnerRML(message);
        }
        requestUpdate();
    }

    // Keyboard and Xbox controllers share a single, deterministic menu cursor.
    // The same navigation actions can later be routed to the real game UI.
    void Navigate(int direction) {
        if (!document_ || direction == 0) return;
        int buttonCount = int(MenuButtonIds.size());
#ifdef KNOMIRA_GAME_SHELL
        if (captureAction_ >= 0) return;
        if (showingSettings_) buttonCount = int(settingsButtonIds_.size());
        if (showingSaves_) buttonCount = int(saves_.size()) + 1; // Back button
        if (confirmingExit_) buttonCount = 2; // Cancel (default), Confirm
#endif
        selectedButton_ = (selectedButton_ + direction + buttonCount) % buttonCount;
        FocusSelected();
    }

    void ActivateSelected() {
        if (!document_) return;
        const std::string id = SelectedId();
        if (Rml::Element* button = document_->GetElementById(id))
            button->Click();
    }

    void BackToMenu() {
#ifdef KNOMIRA_GAME_SHELL
        if (captureAction_ >= 0) {
            CancelCapture();
            return;
        }
        showingSettings_ = false;
        showingSaves_ = false;
        confirmingExit_ = false;
        if (document_) {
            if (auto* main = document_->GetElementById("menu_panel"))
                main->SetProperty("display", "block");
            if (auto* browser = document_->GetElementById("save_browser"))
                browser->SetProperty("display", "none");
            if (auto* confirm = document_->GetElementById("exit_confirm"))
                confirm->SetProperty("display", "none");
            if (auto* settings = document_->GetElementById("settings_panel"))
                settings->SetProperty("display", "none");
        }
#endif
        selectedButton_ = 0;
        FocusSelected();
        SetStatus("Main menu: use your configured bindings, Tab/Shift+Tab, or mouse.");
    }

    void SelectFromClick(const Rml::String& id) {
#ifdef KNOMIRA_GAME_SHELL
        if (showingSettings_) {
            const auto selected = std::find(settingsButtonIds_.begin(), settingsButtonIds_.end(), std::string(id));
            if (selected != settingsButtonIds_.end()) {
                selectedButton_ = int(selected - settingsButtonIds_.begin());
                // Mouse already focuses the clicked element. Calling FocusSelected
                // here would auto-scroll while the click is still in progress.
            }
            return;
        }
        if (confirmingExit_) {
            if (id == "exit_cancel" || id == "exit_yes") {
                selectedButton_ = (id == "exit_yes") ? 1 : 0;
                FocusSelected();
            }
            return;
        }
        if (showingSaves_) {
            if (id == "save_back") {
                selectedButton_ = int(saves_.size());
                FocusSelected();
                return;
            }
            if (id.rfind("save_entry_", 0) == 0) {
                const QString suffix = QString::fromStdString(id.substr(11));
                bool ok = false;
                const int position = suffix.toInt(&ok);
                if (ok && position >= 0 && position < saves_.size()) {
                    selectedButton_ = position;
                    FocusSelected();
                }
                return;
            }
        }
#endif
        for (size_t i = 0; i < MenuButtonIds.size(); ++i) {
            if (id == MenuButtonIds[i]) {
                selectedButton_ = int(i);
                FocusSelected();
                return;
            }
        }
    }

#ifdef KNOMIRA_GAME_SHELL
    void SetStatusText(const QString& plain) {
        const QByteArray utf8 = plain.toHtmlEscaped().toUtf8();
        SetStatus(utf8.constData());
    }

    // A compact, readable table inspired by category-based game keybind menus.
    // Only one category and one input device are shown at a time, so labels
    // and button targets are large enough for mouse and controller users.
    void ShowSettings() {
        if (!document_) return;
        showingSettings_ = true;
        confirmingExit_ = false;
        showingSaves_ = false;
        settingsCategory_ = 0;
        settingsGamepad_ = false;
        if (auto* menu = document_->GetElementById("menu_panel"))
            menu->SetProperty("display", "none");
        if (auto* saves = document_->GetElementById("save_browser"))
            saves->SetProperty("display", "none");
        if (auto* confirm = document_->GetElementById("exit_confirm"))
            confirm->SetProperty("display", "none");
        if (auto* settings = document_->GetElementById("settings_panel"))
            settings->SetProperty("display", "block");
        selectedButton_ = 0;
        RebuildSettings();
        FocusSelected();
        SetStatus("Click a binding once to edit it, or navigate with the controller. Menu inputs are active; gameplay inputs are planned.");
    }

    QString SettingsGroup() const {
        static const char* groups[] = {"Menu", "World (planned)", "Panels (planned)", "System (planned)"};
        return QString::fromUtf8(groups[settingsCategory_]);
    }

    void ChangeSettingsView(const Rml::String& id) {
        const int previousCategory = settingsCategory_;
        const bool previousDevice = settingsGamepad_;
        if (id == "controls_tab_menu") settingsCategory_ = 0;
        else if (id == "controls_tab_world") settingsCategory_ = 1;
        else if (id == "controls_tab_panels") settingsCategory_ = 2;
        else if (id == "controls_tab_system") settingsCategory_ = 3;
        else if (id == "controls_device_keyboard") settingsGamepad_ = false;
        else if (id == "controls_device_xbox") settingsGamepad_ = true;
        else return;
        RebuildSettings();
        // New category/device should start at the first row, not inherit
        // a previous panel's scroll position.
        if (previousCategory != settingsCategory_ || previousDevice != settingsGamepad_) {
            if (auto* list = document_->GetElementById("bindings_list"))
                list->SetScrollTop(0.f);
        }
        SetStatus(settingsCategory_ == 0
            ? "Menu controls take effect immediately. Select a key to change it."
            : "These controls are saved for the full game; gameplay actions are not connected yet.");
    }

    void RebuildSettings() {
        if (!document_) return;
        const std::string oldSelection = SelectedId();
        settingsButtonIds_.clear();
        for (const char* tab : {"controls_tab_menu", "controls_tab_world",
                                "controls_tab_panels", "controls_tab_system",
                                "controls_device_keyboard", "controls_device_xbox"})
            settingsButtonIds_.push_back(tab);

        if (auto* list = document_->GetElementById("bindings_list")) {
            std::string markup;
            const auto& actions = controls_.actions();
            for (int i = 0; i < actions.size(); ++i) {
                const auto& action = actions[i];
                if (action.group != SettingsGroup()) continue;

                markup += "<div class='binding-row'><div class='binding-info'>";
                markup += "<p class='binding-name'>" +
                    action.label.toHtmlEscaped().toUtf8().toStdString() + "</p>";
                markup += action.activeInShell
                    ? "<p class='binding-detail'>ACTIVE IN MENU</p>"
                    : "<p class='binding-planned'>AVAILABLE IN FUTURE GAMEPLAY</p>";
                markup += "</div>";

                for (int slot = 0; slot < 2; ++slot) {
                    const std::string id = std::string(settingsGamepad_ ? "control_p_" : "control_k_") +
                        std::to_string(i) + "_" + std::to_string(slot);
                    settingsButtonIds_.push_back(id);
                    const QString label = settingsGamepad_
                        ? KnomiraInput::Profile::padLabel(action.pads[slot])
                        : KnomiraInput::Profile::keyLabel(action.keys[slot]);
                    const bool unbound = settingsGamepad_ ? action.pads[slot].isEmpty() : action.keys[slot] == 0;
                    // The assignment is visible as a separate, high-contrast
                    // readout rather than relying on a button caption. The
                    // button always says Change and remains the click target.
                    // After FinishCapture(), RebuildSettings() refreshes both
                    // this readout and the active action's input behavior.
                    const QString shown = unbound ? QStringLiteral("Not assigned") : label;
                    markup += "<div class='binding-slot'>";
                    markup += "<p class='binding-value";
                    if (unbound) markup += " unbound";
                    markup += "'>" + shown.toHtmlEscaped().toUtf8().toStdString() + "</p>";
                    markup += "<button class='bind-button' id='" + id + "'>";
                    markup += "Change";
                    markup += "</button></div>";
                }
                markup += "</div>";
            }
            list->SetInnerRML(markup);
            for (const auto& id : settingsButtonIds_) {
                // The first six buttons are statically authored and attached
                // once in Initialise. Only generated fields need listeners.
                if (id.rfind("control_", 0) != 0) continue;
                if (auto* button = document_->GetElementById(id)) {
                    button->AddEventListener("mousedown", &events_); // mouse: capture immediately
                    button->AddEventListener("click", &events_);     // Xbox: Click() route
                }
            }
        }

        for (int i = 0; i < 4; ++i) {
            static const char* categoryIds[] = {
                "controls_tab_menu", "controls_tab_world", "controls_tab_panels", "controls_tab_system"
            };
            if (auto* button = document_->GetElementById(categoryIds[i]))
                button->SetClass("tab-selected", i == settingsCategory_);
        }
        if (auto* button = document_->GetElementById("controls_device_keyboard"))
            button->SetClass("tab-selected", !settingsGamepad_);
        if (auto* button = document_->GetElementById("controls_device_xbox"))
            button->SetClass("tab-selected", settingsGamepad_);

        if (auto* tip = document_->GetElementById("controls_tip")) {
            const QString message = settingsCategory_ == 0
                ? QStringLiteral("Menu: changes work immediately. Click Primary or Alternate, then press the new input.")
                : QStringLiteral("Planned gameplay action: the assignment saves now and activates when the engine is connected.");
            tip->SetInnerRML(message.toHtmlEscaped().toUtf8().toStdString());
        }
        settingsButtonIds_.push_back("controls_reset");
        settingsButtonIds_.push_back("controls_back");
        auto old = std::find(settingsButtonIds_.begin(), settingsButtonIds_.end(), oldSelection);
        selectedButton_ = old == settingsButtonIds_.end() ? 0 : int(old - settingsButtonIds_.begin());
        requestUpdate();
        // Do NOT call FocusSelected() here. Forcing ScrollIntoView while a mouse
        // button is held moves the target between down/up and loses its click.
        // Keyboard/Xbox navigation calls FocusSelected() independently.
    }

    bool IsCapturing() const { return captureAction_ >= 0; }

    void BeginCapture(const Rml::String& id) {
        if (!showingSettings_ || captureAction_ >= 0) return;
        const QStringList parts = QString::fromStdString(id).split('_');
        if (parts.size() != 4 || parts[0] != "control") return;
        bool indexOk = false, slotOk = false;
        const int index = parts[2].toInt(&indexOk);
        const int slot = parts[3].toInt(&slotOk);
        if (!indexOk || !slotOk || slot < 0 || slot > 1 ||
            index < 0 || index >= controls_.actions().size()) return;
        if (parts[1] != "k" && parts[1] != "p") return;
        captureAction_ = index;
        captureSlot_ = slot;
        captureController_ = (parts[1] == "p");
        pendingScrollToFocus_ = false; // Do not move the clicked binding.
        const auto& action = controls_.actions()[index];
        const QString current = captureController_
            ? KnomiraInput::Profile::padLabel(action.pads[slot])
            : KnomiraInput::Profile::keyLabel(action.keys[slot]);
        const QString device = captureController_ ? "Xbox controller" : "keyboard";
        if (auto* title = document_->GetElementById("capture_action_name"))
            title->SetInnerRML(action.label.toHtmlEscaped().toUtf8().toStdString());
        if (auto* previous = document_->GetElementById("capture_current")) {
            const QString caption = QStringLiteral("%1 / %2 binding: %3")
                .arg(device, slot == 0 ? QStringLiteral("Primary") : QStringLiteral("Alternate"), current);
            previous->SetInnerRML(caption.toHtmlEscaped().toUtf8().toStdString());
        }
        if (auto* prompt = document_->GetElementById("capture_prompt")) {
            const QString caption = captureController_
                ? QStringLiteral("Press the Xbox button, trigger or stick direction you want to use.")
                : QStringLiteral("Press the key or combination you want to use.");
            prompt->SetInnerRML(caption.toHtmlEscaped().toUtf8().toStdString());
        }
        if (auto* error = document_->GetElementById("capture_error"))
            error->SetInnerRML("");
        if (auto* modal = document_->GetElementById("binding_capture"))
            modal->SetProperty("display", "block");
        SetStatus("Listening for a new input. Escape or Cancel Change keeps the old binding.");
        requestUpdate();
    }

    void ReportCaptureError(const QString& reason) {
        const QByteArray escaped = reason.toHtmlEscaped().toUtf8();
        if (auto* warning = document_->GetElementById("capture_error"))
            warning->SetInnerRML(escaped.constData());
        SetStatusText(reason);
    }

    void CancelCapture() {
        captureAction_ = -1;
        captureSlot_ = -1;
        captureController_ = false;
        if (document_) {
            if (auto* panel = document_->GetElementById("binding_capture"))
                panel->SetProperty("display", "none");
        }
        FocusSelected();
        SetStatus("Binding capture cancelled. Choose another input or return to menu.");
    }

    void ApplyCapturedKey(int code) {
        if (captureAction_ < 0 || captureController_) return;
        QString issue;
        if (!controls_.bindKey(captureAction_, captureSlot_, code, &issue)) {
            ReportCaptureError("That keyboard shortcut cannot be used: " + issue);
            return;
        }
        FinishCapture();
    }

    void ApplyCapturedPad(const QString& token) {
        if (captureAction_ < 0 || !captureController_) return;
        QString issue;
        if (!controls_.bindPad(captureAction_, captureSlot_, token, &issue)) {
            ReportCaptureError("That controller input cannot be used: " + issue);
            return;
        }
        FinishCapture();
    }

    void ClearCapture() {
        if (captureAction_ < 0) return;
        if (captureController_) ApplyCapturedPad(QString());
        else ApplyCapturedKey(0);
    }

    void FinishCapture() {
        const auto& action = controls_.actions()[captureAction_];
        const QString actionLabel = action.label;
        const QString device = captureController_ ? QStringLiteral("Xbox") : QStringLiteral("Keyboard");
        const QString slot = captureSlot_ == 0 ? QStringLiteral("Primary") : QStringLiteral("Alternate");
        const QString assignment = captureController_
            ? (action.pads[captureSlot_].isEmpty() ? QStringLiteral("Not assigned")
                                                   : KnomiraInput::Profile::padLabel(action.pads[captureSlot_]))
            : (action.keys[captureSlot_] == 0 ? QStringLiteral("Not assigned")
                                               : KnomiraInput::Profile::keyLabel(action.keys[captureSlot_]));
        captureAction_ = -1;
        captureSlot_ = -1;
        captureController_ = false;
        if (auto* panel = document_->GetElementById("binding_capture"))
            panel->SetProperty("display", "none");
        QString error;
        if (!controls_.save(&error))
            SetStatusText("Changed " + actionLabel + " / " + device + " / " + slot +
                          " to " + assignment + ", but saving failed: " + error);
        else
            SetStatusText("Saved " + actionLabel + " / " + device + " / " + slot +
                          ": " + assignment + ". The value is now shown in the table.");
        RebuildSettings();
        FocusSelected(); // Only after capture completes, not during mouse-down.
    }

    void ResetControls() {
        controls_.resetDefaults();
        QString error;
        if (!controls_.save(&error))
            SetStatusText("Default controls restored in memory, but saving failed: " + error);
        else
            SetStatus("All controls restored to defaults and saved.");
        RebuildSettings();
    }

    void OpenExitConfirm() {
        if (!document_) return;
        confirmingExit_ = true;
        showingSaves_ = false;
        showingSettings_ = false;
        if (auto* main = document_->GetElementById("menu_panel"))
            main->SetProperty("display", "none");
        if (auto* browser = document_->GetElementById("save_browser"))
            browser->SetProperty("display", "none");
        if (auto* settings = document_->GetElementById("settings_panel"))
            settings->SetProperty("display", "none");
        if (auto* confirm = document_->GetElementById("exit_confirm"))
            confirm->SetProperty("display", "block");
        selectedButton_ = 0; // Always default to Cancel, including gamepad A.
        FocusSelected();
        SetStatus("Exit Knomira? Cancel is selected. Use your configured Select / Back bindings.");
    }

    void ShowSaveBrowser() {
        saves_ = findIngnomiaSaves();
        showingSaves_ = true;
        confirmingExit_ = false;
        showingSettings_ = false;
        if (auto* main = document_->GetElementById("menu_panel"))
            main->SetProperty("display", "none");
        if (auto* browser = document_->GetElementById("save_browser"))
            browser->SetProperty("display", "block");
        if (auto* list = document_->GetElementById("save_list")) {
            std::string markup;
            if (saves_.isEmpty()) {
                markup = "<p>No Ingnomia saves found in Documents/My Games/Ingnomia/save.</p>";
            }
            for (int i = 0; i < saves_.size(); ++i) {
                const auto& entry = saves_[i];
                const QString text = (entry.kingdomName + " / " + entry.slotName
                    + " (" + entry.version + ")").toHtmlEscaped();
                markup += "<button id='save_entry_" + std::to_string(i) + "'>"
                    + text.toUtf8().toStdString() + "</button>";
            }
            list->SetInnerRML(markup);
            for (int i = 0; i < saves_.size(); ++i) {
                const std::string id = "save_entry_" + std::to_string(i);
                if (auto* button = document_->GetElementById(id))
                    button->AddEventListener("click", &events_);
            }
        }
        selectedButton_ = 0;
        FocusSelected();
        SetStatus("Read-only save browser. Selecting a save shows its details; simulation loading comes next.");
    }

    void ShowSaveDetail(int index) {
        if (index < 0 || index >= saves_.size()) return;
        const auto& entry = saves_[index];
        const QString info = "Selected: " + entry.kingdomName + " / " + entry.slotName
            + " | Version: " + entry.version + " | Path: " + entry.absolutePath
            + " | Read-only preview; game engine not connected.";
        const QByteArray escaped = info.toHtmlEscaped().toUtf8();
        SetStatus(escaped.constData());
    }
#endif

protected:
    void exposeEvent(QExposeEvent* event) override {
        QWindow::exposeEvent(event);
        if (isExposed()) requestUpdate();
    }

    void resizeEvent(QResizeEvent* event) override {
        QWindow::resizeEvent(event);
        ResizeUi();
        requestUpdate();
    }

    bool event(QEvent* event) override {
        if (event->type() == QEvent::UpdateRequest) {
            Render();
            return true;
        }
        return QWindow::event(event);
    }

    void mouseMoveEvent(QMouseEvent* event) override {
        if (context_) {
            const double scale = devicePixelRatio();
            context_->ProcessMouseMove(
                int(std::lround(event->position().x() * scale)),
                int(std::lround(event->position().y() * scale)),
                Modifiers(event->modifiers()));
        }
        requestUpdate();
    }

    void mousePressEvent(QMouseEvent* event) override {
#ifdef KNOMIRA_GAME_SHELL
        pendingScrollToFocus_ = false; // Never auto-scroll under a mouse click.
#endif
        if (context_) {
            const double scale = devicePixelRatio();
            context_->ProcessMouseMove(
                int(std::lround(event->position().x() * scale)),
                int(std::lround(event->position().y() * scale)),
                Modifiers(event->modifiers()));
            const int button = MouseButton(event->button());
            if (button >= 0)
                context_->ProcessMouseButtonDown(button, Modifiers(event->modifiers()));
        }
        requestUpdate();
    }

    void mouseReleaseEvent(QMouseEvent* event) override {
        if (context_) {
            // RmlUi's click requires down/up over the same element. Update
            // cursor coordinates even when Qt did not emit a move in between.
            const double scale = devicePixelRatio();
            context_->ProcessMouseMove(
                int(std::lround(event->position().x() * scale)),
                int(std::lround(event->position().y() * scale)),
                Modifiers(event->modifiers()));
            const int button = MouseButton(event->button());
            if (button >= 0)
                context_->ProcessMouseButtonUp(button, Modifiers(event->modifiers()));
        }
        requestUpdate();
    }

    void wheelEvent(QWheelEvent* event) override {
        if (context_) {
            const QPoint delta = event->angleDelta();
            context_->ProcessMouseWheel(
                Rml::Vector2f(-delta.x() / 120.f, -delta.y() / 120.f),
                Modifiers(event->modifiers()));
        }
        requestUpdate();
    }

    void keyPressEvent(QKeyEvent* event) override {
#ifdef KNOMIRA_GAME_SHELL
        if (captureAction_ >= 0) {
            if (!event->isAutoRepeat()) {
                if (event->key() == Qt::Key_Escape) {
                    CancelCapture();
                } else if (!captureController_) {
                    switch (event->key()) {
                    case Qt::Key_Shift:
                    case Qt::Key_Control:
                    case Qt::Key_Alt:
                    case Qt::Key_Meta:
                        break; // Wait for a complete chord.
                    default:
                        ApplyCapturedKey(KnomiraInput::Profile::normalizeKey(
                            event->key(), int(event->modifiers())));
                        break;
                    }
                }
            }
            event->accept();
            return;
        }
        if (!event->isAutoRepeat()) {
            const int chord = KnomiraInput::Profile::normalizeKey(
                event->key(), int(event->modifiers()));
            if (event->key() == Qt::Key_Escape) BackToMenu(); // always recover
            else if (event->key() == Qt::Key_Tab || event->key() == Qt::Key_Backtab)
                Navigate((event->key() == Qt::Key_Backtab ||
                          (event->modifiers() & Qt::ShiftModifier)) ? -1 : 1);
            else if (controls_.keyMatches("menu_prev", chord)) Navigate(-1);
            else if (controls_.keyMatches("menu_next", chord)) Navigate(1);
            else if (controls_.keyMatches("menu_accept", chord)) ActivateSelected();
            else if (controls_.keyMatches("menu_back", chord)) BackToMenu();
            else {
                event->ignore();
                return;
            }
        }
        event->accept();
        return;
#else
        if (!event->isAutoRepeat()) {
            switch (event->key()) {
            case Qt::Key_Tab:
                Navigate(event->modifiers() & Qt::ShiftModifier ? -1 : 1);
                event->accept();
                return;
            case Qt::Key_Backtab:
                Navigate(-1);
                event->accept();
                return;
            case Qt::Key_Up:
            case Qt::Key_Left:
                Navigate(-1);
                event->accept();
                return;
            case Qt::Key_Down:
            case Qt::Key_Right:
                Navigate(1);
                event->accept();
                return;
            case Qt::Key_Return:
            case Qt::Key_Enter:
            case Qt::Key_Space:
                ActivateSelected();
                event->accept();
                return;
            case Qt::Key_Escape:
                BackToMenu();
                event->accept();
                return;
            default:
                break;
            }
        }
        if (context_) {
            const int modifiers = Modifiers(event->modifiers());
            const auto key = Key(event->key());
            if (key != Rml::Input::KI_UNKNOWN)
                context_->ProcessKeyDown(key, modifiers);
            if (!(event->modifiers() & (Qt::ControlModifier | Qt::AltModifier | Qt::MetaModifier))
                && !event->text().isEmpty()) {
                context_->ProcessTextInput(event->text().toUtf8().toStdString());
            }
        }
        requestUpdate();
#endif
    }

    void keyReleaseEvent(QKeyEvent* event) override {
        if (context_) {
            const auto key = Key(event->key());
            if (key != Rml::Input::KI_UNKNOWN)
                context_->ProcessKeyUp(key, Modifiers(event->modifiers()));
        }
        requestUpdate();
    }

private:
    std::string SelectedId() const {
#ifdef KNOMIRA_GAME_SHELL
        if (confirmingExit_)
            return selectedButton_ == 1 ? "exit_yes" : "exit_cancel";
        if (showingSaves_) {
            if (selectedButton_ < saves_.size())
                return "save_entry_" + std::to_string(selectedButton_);
            return "save_back";
        }
        if (showingSettings_ && selectedButton_ >= 0 && selectedButton_ < int(settingsButtonIds_.size()))
            return settingsButtonIds_[size_t(selectedButton_)];
#endif
        return MenuButtonIds[size_t(selectedButton_)];
    }

    void FocusSelected() {
        if (!document_) return;
        const std::string id = SelectedId();
        if (Rml::Element* button = document_->GetElementById(id)) {
            button->Focus(true); // Enables CSS :focus-visible highlight.
#ifdef KNOMIRA_GAME_SHELL
            if (showingSettings_) pendingScrollToFocus_ = true;
#endif
        }
        requestUpdate();
    }

    void PollGamepad() {
#ifdef _WIN32
        if (!isActive() || !document_) return;
        const qint64 now = pollClock_.elapsed();
        XINPUT_STATE state{};
        DWORD result = ERROR_DEVICE_NOT_CONNECTED;

        if (controllerIndex_ < XUSER_MAX_COUNT)
            result = ReadControllerState(controllerIndex_, &state);

        if (result != ERROR_SUCCESS) {
            controllerIndex_ = XUSER_MAX_COUNT;
            previousButtons_ = 0;
            heldDirection_ = 0;
#ifdef KNOMIRA_GAME_SHELL
            lastPadTokens_.clear();
#endif
            if (now < nextScanMs_) return;
            nextScanMs_ = now + 250; // Cheap polling when no gamepad exists.
            for (DWORD i = 0; i < XUSER_MAX_COUNT; ++i) {
                if (ReadControllerState(i, &state) == ERROR_SUCCESS) {
                    controllerIndex_ = i;
                    result = ERROR_SUCCESS;
#ifdef KNOMIRA_GAME_SHELL
                    SetStatus("Xbox controller detected. Custom bindings available in Settings / Controls.");
#else
                    SetStatus("Xbox controller detected. D-pad / left stick: navigate; A: select; B: back.");
#endif
                    break;
                }
            }
        }

        if (result != ERROR_SUCCESS) return;

        const WORD buttons = state.Gamepad.wButtons;
        const WORD pressed = static_cast<WORD>(buttons & ~previousButtons_);
        previousButtons_ = buttons;

#ifdef KNOMIRA_GAME_SHELL
        // Convert physical XInput state into logical tokens. These exact names
        // are persisted in controls.json and shared with future world actions.
        QSet<QString> held;
        auto add = [&held](bool on, const char* token) {
            if (on) held.insert(QString::fromLatin1(token));
        };
        add((buttons & XINPUT_GAMEPAD_DPAD_UP) != 0, "DPadUp");
        add((buttons & XINPUT_GAMEPAD_DPAD_DOWN) != 0, "DPadDown");
        add((buttons & XINPUT_GAMEPAD_DPAD_LEFT) != 0, "DPadLeft");
        add((buttons & XINPUT_GAMEPAD_DPAD_RIGHT) != 0, "DPadRight");
        add((buttons & XINPUT_GAMEPAD_A) != 0, "A");
        add((buttons & XINPUT_GAMEPAD_B) != 0, "B");
        add((buttons & XINPUT_GAMEPAD_X) != 0, "X");
        add((buttons & XINPUT_GAMEPAD_Y) != 0, "Y");
        add((buttons & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0, "LB");
        add((buttons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0, "RB");
        add((buttons & XINPUT_GAMEPAD_BACK) != 0, "Back");
        add((buttons & XINPUT_GAMEPAD_START) != 0, "Start");
        add((buttons & XINPUT_GAMEPAD_LEFT_THUMB) != 0, "LThumb");
        add((buttons & XINPUT_GAMEPAD_RIGHT_THUMB) != 0, "RThumb");
        constexpr SHORT stickThreshold = 16000; // Exceeds normal stick drift.
        add(state.Gamepad.sThumbLY > stickThreshold, "LSUp");
        add(state.Gamepad.sThumbLY < -stickThreshold, "LSDown");
        add(state.Gamepad.sThumbLX < -stickThreshold, "LSLeft");
        add(state.Gamepad.sThumbLX > stickThreshold, "LSRight");
        add(state.Gamepad.sThumbRY > stickThreshold, "RSUp");
        add(state.Gamepad.sThumbRY < -stickThreshold, "RSDown");
        add(state.Gamepad.sThumbRX < -stickThreshold, "RSLeft");
        add(state.Gamepad.sThumbRX > stickThreshold, "RSRight");
        add(state.Gamepad.bLeftTrigger > 140, "LT");
        add(state.Gamepad.bRightTrigger > 140, "RT");

        QSet<QString> justPressed = held;
        for (const QString& old : lastPadTokens_) justPressed.remove(old);
        lastPadTokens_ = held;

        if (captureAction_ >= 0) {
            if (captureController_) {
                // Use deterministic order if a player presses multiple inputs.
                for (const QString& token : KnomiraInput::Profile::availablePadTokens()) {
                    if (justPressed.contains(token)) {
                        ApplyCapturedPad(token);
                        break;
                    }
                }
            } else if (justPressed.contains("B") || justPressed.contains("Back")) {
                // A controller can escape keyboard-only capture without a
                // physical keyboard. Controller captures still allow B/Back.
                CancelCapture();
            }
            return;
        }

        int direction = 0;
        if (controls_.padMatches("menu_prev", held)) direction = -1;
        else if (controls_.padMatches("menu_next", held)) direction = 1;
#else
        // Unmodified Qt host prototype keeps its verified fixed XInput controls.
        constexpr SHORT stickThreshold = 16000;
        int direction = 0;
        if ((buttons & XINPUT_GAMEPAD_DPAD_UP) || state.Gamepad.sThumbLY > stickThreshold)
            direction = -1;
        else if ((buttons & XINPUT_GAMEPAD_DPAD_DOWN) || state.Gamepad.sThumbLY < -stickThreshold)
            direction = 1;
#endif
        if (direction == 0) {
            heldDirection_ = 0;
        } else if (direction != heldDirection_) {
            heldDirection_ = direction;
            nextRepeatMs_ = now + 360;
            Navigate(direction);
        } else if (now >= nextRepeatMs_) {
            nextRepeatMs_ = now + 140;
            Navigate(direction);
        }

#ifdef KNOMIRA_GAME_SHELL
        if (controls_.padMatches("menu_accept", justPressed)) {
            ActivateSelected();
            return;
        }
        if (controls_.padMatches("menu_back", justPressed))
            BackToMenu();
#else
        if (pressed & XINPUT_GAMEPAD_A) {
            ActivateSelected();
            return;
        }
        if (pressed & XINPUT_GAMEPAD_B) BackToMenu();
#endif
#endif
    }

    void ResizeUi() {
        if (!context_ || !renderer_) return;
        const QSize pixels = size() * devicePixelRatio();
        if (pixels.width() <= 0 || pixels.height() <= 0) return;
        context_->SetDimensions(Rml::Vector2i(pixels.width(), pixels.height()));
        context_->SetDensityIndependentPixelRatio(float(devicePixelRatio()));
        renderer_->SetViewport(pixels.width(), pixels.height());
    }

    bool Initialise() {
        glContext_ = std::make_unique<QOpenGLContext>();
        glContext_->setFormat(format());
        if (!glContext_->create() || !glContext_->makeCurrent(this)) {
            qCritical() << "Cannot create a Qt OpenGL context.";
            return false;
        }
        Rml::String glMessage;
        if (!RmlGL3::Initialize(&glMessage)) {
            qCritical() << "RmlUi OpenGL function loader failed.";
            return false;
        }
        glLoaderInitialised_ = true;
        renderer_ = std::make_unique<RenderInterface_GL3>();
        if (!*renderer_) {
            qCritical() << "RmlUi GL3 renderer setup failed.";
            return false;
        }
        Rml::SetSystemInterface(&system_);
        Rml::SetRenderInterface(renderer_.get());
        if (!Rml::Initialise()) {
            qCritical() << "RmlUi initialization failed.";
            return false;
        }
        rmlInitialised_ = true;
        context_ = Rml::CreateContext("knomira-qt-host", Rml::Vector2i(1280, 800));
        if (!context_ || !LoadFont()) {
            qCritical() << "RmlUi context/font setup failed.";
            return false;
        }
        ResizeUi();
#ifdef KNOMIRA_GAME_SHELL
        const QString filePath = QDir::fromNativeSeparators(
            QCoreApplication::applicationDirPath() + "/game_shell.rml");
#else
        const QString filePath = QDir::fromNativeSeparators(
            QCoreApplication::applicationDirPath() + "/main_menu.rml");
#endif
        document_ = context_->LoadDocument(filePath.toStdString());
        if (!document_) {
            qCritical() << "Cannot load main menu:" << filePath;
            return false;
        }
        for (const char* id : MenuButtonIds) {
            if (Rml::Element* element = document_->GetElementById(id))
                element->AddEventListener("click", &events_);
        }
        document_->Show();
        if (Rml::Element* footer = document_->GetElementById("footer"))
#ifdef KNOMIRA_GAME_SHELL
            footer->SetInnerRML("Noesis-free shell | Original game simulation not connected yet.");
#else
            footer->SetInnerRML("Proof of concept: Qt 6 + RmlUi + OpenGL; no NoesisGUI.");
#endif
#ifdef KNOMIRA_GAME_SHELL
        if (auto* back = document_->GetElementById("save_back"))
            back->AddEventListener("click", &events_);
        for (const char* id : {"exit_cancel", "exit_yes",
                               "controls_reset", "controls_back",
                               "controls_tab_menu", "controls_tab_world",
                               "controls_tab_panels", "controls_tab_system",
                               "controls_device_keyboard", "controls_device_xbox",
                               "capture_clear", "capture_cancel"}) {
            if (auto* option = document_->GetElementById(id))
                option->AddEventListener("click", &events_);
        }
#endif
        SetStatus("Use arrows / Tab or Xbox D-pad / stick to navigate. Enter / A selects.");
        selectedButton_ = 0;
        FocusSelected();
        glContext_->doneCurrent();
        qInfo() << "Knomira Qt-hosted RmlUi menu initialized.";
        return true;
    }

    void Render() {
        if (!isExposed() || failed_) return;
        if (!glContext_) {
            if (!Initialise()) {
                failed_ = true;
                return;
            }
        }
        if (!context_ || !glContext_->makeCurrent(this)) return;
        context_->Update();
#ifdef KNOMIRA_GAME_SHELL
        // Layout must be computed before scrolling to a dynamically-generated
        // row. This keeps every binding reachable with only an Xbox controller.
        if (pendingScrollToFocus_ && document_) {
            const std::string id = SelectedId();
            if (Rml::Element* button = document_->GetElementById(id))
                button->ScrollIntoView();
            pendingScrollToFocus_ = false;
        }
#endif
        renderer_->Clear();
        renderer_->BeginFrame();
        context_->Render();
        renderer_->EndFrame();
        glContext_->swapBuffers(this);
        glContext_->doneCurrent();
    }

    QtSystemInterface system_;
    MenuEvents events_;
    QTimer timer_;
    QElapsedTimer pollClock_;
    int selectedButton_ = 0;
#ifdef KNOMIRA_GAME_SHELL
    QVector<KnomiraSaveEntry> saves_;
    bool showingSaves_ = false;
    bool confirmingExit_ = false;
    bool showingSettings_ = false;
    int settingsCategory_ = 0; // Menu, World, Panels, System
    bool settingsGamepad_ = false; // Keyboard or Xbox columns
    KnomiraInput::Profile controls_;
    std::vector<std::string> settingsButtonIds_;
    int captureAction_ = -1;
    int captureSlot_ = -1;
    bool captureController_ = false;
    bool pendingScrollToFocus_ = false;
    QSet<QString> lastPadTokens_;
#endif
#ifdef _WIN32
    DWORD controllerIndex_ = XUSER_MAX_COUNT;
    WORD previousButtons_ = 0;
    int heldDirection_ = 0;
    qint64 nextRepeatMs_ = 0;
    qint64 nextScanMs_ = 0;
#endif
    std::unique_ptr<QOpenGLContext> glContext_;
    std::unique_ptr<RenderInterface_GL3> renderer_;
    Rml::Context* context_ = nullptr;
    Rml::ElementDocument* document_ = nullptr;
    bool glLoaderInitialised_ = false;
    bool rmlInitialised_ = false;
    bool failed_ = false;
};

void MenuEvents::ProcessEvent(Rml::Event& event) {
    if (!window_) return;
    Rml::Element* element = event.GetCurrentElement();
    if (!element) return;
    const Rml::String& id = element->GetId();

#ifdef KNOMIRA_GAME_SHELL
    // Mouse activation uses mousedown to avoid the RmlUi click requirement
    // (press+release over the same element). This fixes rows that previously
    // scrolled into view but did not enter the capture dialog.
    // Xbox activation still calls Click(), so both methods work.
    if (event.GetType() == "mousedown") {
        if (id.rfind("control_", 0) == 0 &&
            event.GetParameter<int>("button", -1) == 0 &&
            !window_->IsCapturing()) {
            window_->SelectFromClick(id);
            window_->BeginCapture(id);
            event.StopPropagation();
        }
        return;
    }
    if (event.GetType() != "click") return;
    if (window_->IsCapturing()) {
        // The modal owns input. Underlying controls must not activate.
        if (id == "capture_cancel") window_->CancelCapture();
        else if (id == "capture_clear") window_->ClearCapture();
        return;
    }
#endif

    window_->SelectFromClick(id);
    if (id == "quit") {
#ifdef KNOMIRA_GAME_SHELL
        window_->OpenExitConfirm();
#else
        window_->close();
#endif
    } else if (id == "new_game") {
        window_->SetStatus("New Game: world generation will be wired after simulation bootstrap.");
    } else if (id == "load_game") {
#ifdef KNOMIRA_GAME_SHELL
        window_->ShowSaveBrowser();
#else
        window_->SetStatus("Qt host: Load Game clicked; saves not connected.");
#endif
    } else if (id == "settings") {
#ifdef KNOMIRA_GAME_SHELL
        window_->ShowSettings();
#else
        window_->SetStatus("Settings: engine settings adapter not connected yet.");
#endif
#ifdef KNOMIRA_GAME_SHELL
    } else if (id == "save_back" || id == "exit_cancel" || id == "controls_back") {
        window_->BackToMenu();
    } else if (id == "controls_reset") {
        window_->ResetControls();
    } else if (id == "capture_cancel") {
        window_->CancelCapture();
    } else if (id == "capture_clear") {
        window_->ClearCapture();
    } else if (id.rfind("controls_tab_", 0) == 0 ||
               id.rfind("controls_device_", 0) == 0) {
        window_->ChangeSettingsView(id);
    } else if (id.rfind("control_", 0) == 0) {
        window_->BeginCapture(id); // Controller/keyboard activation (Click).
    } else if (id == "exit_yes") {
        window_->close();
    } else if (id.rfind("save_entry_", 0) == 0) {
        bool valid = false;
        const int index = QString::fromStdString(id.substr(11)).toInt(&valid);
        if (valid) window_->ShowSaveDetail(index);
#endif
    }
}

} // namespace

int main(int argc, char** argv) {
    QGuiApplication application(argc, argv);
#ifdef KNOMIRA_GAME_SHELL
    QCoreApplication::setOrganizationName(QStringLiteral("Knomira"));
    QCoreApplication::setApplicationName(QStringLiteral("Knomira"));
#endif
    QtMenuWindow window;
    window.show();
    return application.exec();
}
