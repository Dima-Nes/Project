#pragma once
#include "State.h"
#include "World.h"
#include "Player.h"
#include "HUD.h"
#include "Database.h"
#include <string>

using namespace sf;

class PlayState : public State {
private:
    World   world;
    Player  player;
    HUD     hud;
    View    camera;

    Database* db;
    std::string username;

    // ─── Пауза ────────────────────────────────────────────────────────────────
    bool paused = false;

    Font        pauseFont;
    RectangleShape pauseOverlay;  // полупрозрачный фон
    Text        pauseTitle;
    Text        btnResume;        // Продолжить
    Text        btnBackToMenu;    // Выйти в меню

    Clock pauseAnimClock;

    void buildPauseMenu(RenderWindow& window);
    void renderPauseMenu(RenderWindow& window);
    // Возвращает 0 если нужно выйти в меню, -1 иначе
    int  updatePauseMenu(RenderWindow& window, Event& event);

    // ─── Камера ───────────────────────────────────────────────────────────────
    void updateCamera(RenderWindow& window);

    // ─── Сохранение ───────────────────────────────────────────────────────────
    void saveProgress();

public:
    // savedSeed == -1  → генерировать новый мир
    // spawnX/Y == -1   → ставить игрока на стандартный спавн
    PlayState(Database* db, const std::string& username,
        int savedSeed = -1, float spawnX = -1.f, float spawnY = -1.f);

    // 0 = назад в GameMenu,  -1 = продолжаем
    int update(RenderWindow& window, Event& event) override;

    void updateLogic(RenderWindow& window, float dt);
    void render(RenderWindow& window) override;
};