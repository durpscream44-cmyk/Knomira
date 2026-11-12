// Copyright (C) 2026 Knomira contributors.
// Optional RmlUi overlay rendered in the EXISTING game OpenGL context.
// No QWindow or QOpenGLContext is created by this class.
#pragma once

#include <memory>

class QString;

namespace Knomira {

class RmlUiEngineOverlay final
{
public:
    RmlUiEngineOverlay();
    ~RmlUiEngineOverlay();

    RmlUiEngineOverlay(const RmlUiEngineOverlay&) = delete;
    RmlUiEngineOverlay& operator=(const RmlUiEngineOverlay&) = delete;

    // Requires the game's initialized OpenGL context to be current.
    // Only one RmlUiEngineOverlay may be active per process.
    bool initialize(const QString& documentPath, int pixelWidth, int pixelHeight, float devicePixelRatio);
    void resize(int pixelWidth, int pixelHeight, float devicePixelRatio);
    void render(); // UI only: never advances gameplay or simulation ticks.
    void shutdown(); // Requires a current OpenGL context to release GPU resources.

    bool isReady() const;

private:
    struct State;
    std::unique_ptr<State> state_;
};

} // namespace Knomira
