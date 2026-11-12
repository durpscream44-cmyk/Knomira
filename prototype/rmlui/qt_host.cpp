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
        setTitle(QStringLiteral("Knomira | Qt + RmlUi Host"));
        resize(1280, 800);

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
        constexpr int buttonCount = int(MenuButtonIds.size());
        selectedButton_ = (selectedButton_ + direction + buttonCount) % buttonCount;
        FocusSelected();
    }

    void ActivateSelected() {
        if (!document_) return;
        if (Rml::Element* button = document_->GetElementById(MenuButtonIds[size_t(selectedButton_)]))
            button->Click();
    }

    void BackToMenu() {
        selectedButton_ = 0;
        FocusSelected();
        SetStatus("Main menu: use arrows / D-pad / left stick, Enter / A to select.");
    }

    void SelectFromClick(const Rml::String& id) {
        for (size_t i = 0; i < MenuButtonIds.size(); ++i) {
            if (id == MenuButtonIds[i]) {
                selectedButton_ = int(i);
                FocusSelected();
                return;
            }
        }
    }

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
    void FocusSelected() {
        if (!document_) return;
        if (Rml::Element* button = document_->GetElementById(MenuButtonIds[size_t(selectedButton_)]))
            button->Focus(true); // Enables CSS :focus-visible highlight.
        requestUpdate();
    }

    void PollGamepad() {
#ifdef _WIN32
        if (!isActive() || !document_) return;
        const qint64 now = pollClock_.elapsed();
        XINPUT_STATE state{};
        DWORD result = ERROR_DEVICE_NOT_CONNECTED;

        if (controllerIndex_ < XUSER_MAX_COUNT)
            result = XInputGetState(controllerIndex_, &state);

        if (result != ERROR_SUCCESS) {
            controllerIndex_ = XUSER_MAX_COUNT;
            previousButtons_ = 0;
            heldDirection_ = 0;
            if (now < nextScanMs_) return;
            nextScanMs_ = now + 250; // Cheap polling when no gamepad exists.
            for (DWORD i = 0; i < XUSER_MAX_COUNT; ++i) {
                if (XInputGetState(i, &state) == ERROR_SUCCESS) {
                    controllerIndex_ = i;
                    result = ERROR_SUCCESS;
                    SetStatus("Xbox controller detected. D-pad / left stick: navigate; A: select; B: back.");
                    break;
                }
            }
        }

        if (result != ERROR_SUCCESS) return;

        const WORD buttons = state.Gamepad.wButtons;
        const WORD pressed = static_cast<WORD>(buttons & ~previousButtons_);
        previousButtons_ = buttons;

        // XInput stick thresholds are intentionally higher than hardware drift.
        constexpr SHORT stickThreshold = 16000;
        int direction = 0;
        if ((buttons & XINPUT_GAMEPAD_DPAD_UP) || state.Gamepad.sThumbLY > stickThreshold)
            direction = -1;
        else if ((buttons & XINPUT_GAMEPAD_DPAD_DOWN) || state.Gamepad.sThumbLY < -stickThreshold)
            direction = 1;

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

        if (pressed & XINPUT_GAMEPAD_A) {
            ActivateSelected();
            // The window may close on Exit. Do not touch its UI again in this tick.
            return;
        }
        if (pressed & XINPUT_GAMEPAD_B)
            BackToMenu();
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
        const QString filePath = QDir::fromNativeSeparators(
            QCoreApplication::applicationDirPath() + "/main_menu.rml");
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
            footer->SetInnerRML("Proof of concept: Qt 6 + RmlUi + OpenGL; no NoesisGUI.");
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
    window_->SelectFromClick(id);
    if (id == "quit") {
        window_->close();
    } else if (id == "new_game") {
        window_->SetStatus("Qt host: New Game clicked; simulation not connected.");
    } else if (id == "load_game") {
        window_->SetStatus("Qt host: Load Game clicked; saves not connected.");
    } else if (id == "settings") {
        window_->SetStatus("Qt host: Settings clicked; game preferences not connected.");
    }
}

} // namespace

int main(int argc, char** argv) {
    QGuiApplication application(argc, argv);
    QtMenuWindow window;
    window.show();
    return application.exec();
}
