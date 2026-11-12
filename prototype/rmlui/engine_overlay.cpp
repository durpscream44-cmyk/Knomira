// Copyright (C) 2026 Knomira contributors.
// NOTE: This source is compiled only when the game's optional RmlUi overlay
// experiment is enabled (or by the standalone Qt adapter compile test).
#include "engine_overlay.h"

#include <RmlUi/Core.h>
#include <RmlUi/Core/Context.h>
#include <RmlUi/Core/ElementDocument.h>
#include <RmlUi/Core/SystemInterface.h>
#include <RmlUi_Renderer_GL3.h>

#include <QClipboard>
#include <QElapsedTimer>
#include <QGuiApplication>
#include <QOpenGLContext>
#include <QDebug>
#include <QString>

#include <algorithm>

namespace Knomira {
namespace {

class QtSystemInterface final : public Rml::SystemInterface {
public:
    QtSystemInterface() { elapsed_.start(); }

    double GetElapsedTime() override { return elapsed_.elapsed() / 1000.0; }

    void SetClipboardText(const Rml::String& text) override {
        QGuiApplication::clipboard()->setText(QString::fromUtf8(text.data(), int(text.size())));
    }

    void GetClipboardText(Rml::String& text) override {
        const QByteArray bytes = QGuiApplication::clipboard()->text().toUtf8();
        text.assign(bytes.constData(), size_t(bytes.size()));
    }
private:
    QElapsedTimer elapsed_;
};

bool LoadDefaultFont() {
#ifdef _WIN32
    constexpr const char* candidates[] = {
        "C:/Windows/Fonts/segoeui.ttf",
        "C:/Windows/Fonts/arial.ttf"
    };
#else
    constexpr const char* candidates[] = {
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/liberation2/LiberationSans-Regular.ttf"
    };
#endif
    for (const char* candidate : candidates) {
        if (Rml::LoadFontFace(candidate)) return true;
    }
    return false;
}

} // namespace

struct RmlUiEngineOverlay::State {
    QtSystemInterface system;
    std::unique_ptr<RenderInterface_GL3> renderer;
    Rml::Context* context = nullptr;
    Rml::ElementDocument* document = nullptr;
    bool rmlInitialized = false;
};

RmlUiEngineOverlay::RmlUiEngineOverlay() = default;

RmlUiEngineOverlay::~RmlUiEngineOverlay() {
    // MainWindow must make its existing GL context current before destruction.
    shutdown();
}

bool RmlUiEngineOverlay::initialize(const QString& documentPath, int pixelWidth, int pixelHeight, float devicePixelRatio) {
    if (state_) return false;
    if (!QOpenGLContext::currentContext()) {
        qWarning() << "RmlUi overlay initialization requires the game's active OpenGL context.";
        return false;
    }

    state_ = std::make_unique<State>();
    Rml::String rendererMessage;
    // The game-specific build uses RMLUI_GL3_CUSTOM_LOADER=<glad/gl.h> so
    // RmlUi reuses the game's existing GLAD loader, not a second global GLAD.
    if (!RmlGL3::Initialize(&rendererMessage)) {
        qWarning() << "RmlUi overlay GL3 initialization failed:" << rendererMessage.c_str();
        shutdown();
        return false;
    }

    state_->renderer = std::make_unique<RenderInterface_GL3>();
    if (!static_cast<bool>(*state_->renderer)) {
        qWarning() << "RmlUi overlay failed to initialize its GL3 renderer.";
        shutdown();
        return false;
    }

    Rml::SetSystemInterface(&state_->system);
    Rml::SetRenderInterface(state_->renderer.get());
    if (!Rml::Initialise()) {
        qWarning() << "RmlUi overlay core initialization failed.";
        shutdown();
        return false;
    }
    state_->rmlInitialized = true;

    state_->context = Rml::CreateContext("knomira-engine-overlay", Rml::Vector2i(1, 1));
    if (!state_->context || !LoadDefaultFont()) {
        qWarning() << "RmlUi overlay failed to create a context or load a font.";
        shutdown();
        return false;
    }

    resize(pixelWidth, pixelHeight, devicePixelRatio);
    const QByteArray utf8Path = documentPath.toUtf8();
    state_->document = state_->context->LoadDocument(utf8Path.constData());
    if (!state_->document) {
        qWarning() << "RmlUi overlay document not found:" << documentPath;
        shutdown();
        return false;
    }
    state_->document->Show();
    qInfo() << "Knomira in-engine RmlUi overlay initialized (passive rendering, no input capture).";
    return true;
}

void RmlUiEngineOverlay::resize(int pixelWidth, int pixelHeight, float devicePixelRatio) {
    if (!state_ || !state_->context || !state_->renderer) return;
    const int w = std::max(pixelWidth, 1);
    const int h = std::max(pixelHeight, 1);
    state_->context->SetDimensions(Rml::Vector2i(w, h));
    state_->context->SetDensityIndependentPixelRatio(std::max(devicePixelRatio, 0.5f));
    state_->renderer->SetViewport(w, h);
}

void RmlUiEngineOverlay::render() {
    if (!isReady()) return;
    if (!QOpenGLContext::currentContext()) return;
    state_->context->Update();
    // Do not clear the framebuffer: the game world and Noesis are already drawn.
    state_->renderer->BeginFrame();
    state_->context->Render();
    state_->renderer->EndFrame();
}

void RmlUiEngineOverlay::shutdown() {
    if (!state_) return;
    if (!QOpenGLContext::currentContext()) {
        qWarning() << "RmlUi overlay must be destroyed while the game GL context is current.";
        // Do not touch GPU objects without their context. Caller must shut down
        // before MainWindow destroys QOpenGLContext.
        return;
    }
    if (state_->rmlInitialized) {
        Rml::Shutdown();
        state_->rmlInitialized = false;
    }
    state_->renderer.reset();
    RmlGL3::Shutdown();
    state_.reset();
}

bool RmlUiEngineOverlay::isReady() const {
    return state_ && state_->rmlInitialized && state_->context && state_->document;
}

} // namespace Knomira
