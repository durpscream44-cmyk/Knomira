// Copyright (C) 2026 Knomira contributors.
// PLAY-01F: development-only bridge to the *original* game services.
// This is not a shipping save path, a world renderer, or an automated agent.
#pragma once

#include <QString>
#include <memory>

class Config;
class GameManager;
class QTemporaryDir;

class KnomiraShellEngineProbe final {
public:
    KnomiraShellEngineProbe() = default;
    ~KnomiraShellEngineProbe();
    KnomiraShellEngineProbe(const KnomiraShellEngineProbe&) = delete;
    KnomiraShellEngineProbe& operator=(const KnomiraShellEngineProbe&) = delete;

    // Creates a marked disposable root and the original Config/DB/GameManager.
    // No world generation and no reads of production player-save directories.
    bool prepare(QString* error);
    // Requires explicitly supplied, authorized tilesheets; test colony only.
    // Deliberately synchronous while rendering/thread integration is unverified.
    bool startNewGame(QString* error);
    bool hasGame() const;
    QString saveRoot() const;

private:
    std::unique_ptr<QTemporaryDir> root_;
    std::unique_ptr<Config> config_;
    std::unique_ptr<GameManager> manager_;
};
