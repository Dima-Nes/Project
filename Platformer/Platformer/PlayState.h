#pragma once
#include "State.h"
#include "World.h"
#include "Player.h"
#include "HUD.h"
#include "Database.h"
#include <string>
#include "Background.h"

using namespace sf;

class PlayState : public State {
private:
    Background background;
    World   world;
    Player  player;
    HUD     hud;
    View    camera;

    Database* db;
    std::string username;

    // ─── Пауза ────────────────────────────────────────────────────────────────
    bool paused = false;
    bool pauseBuilt = false; // ← обычный member, не static!

    Font           pauseFont;
    RectangleShape pauseOverlay;
    Text           pauseTitle;
    Text           btnResume;
    Text           btnBackToMenu;
    Clock          pauseAnimClock;

    void buildPauseMenu(RenderWindow& window);
    void renderPauseMenu(RenderWindow& window);
    int  updatePauseMenu(RenderWindow& window, Event& event);

    // ─── Камера ───────────────────────────────────────────────────────────────
    void updateCamera(RenderWindow& window);

    // ─── Сохранение ───────────────────────────────────────────────────────────
    void saveProgress();

public:
    PlayState(Database* db, const std::string& username,
        int savedSeed = -1, float spawnX = -1.f, float spawnY = -1.f);

    int  update(RenderWindow& window, Event& event) override;
    void updateLogic(RenderWindow& window, float dt);
    void render(RenderWindow& window) override;
};