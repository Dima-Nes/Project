#pragma once
#include "State.h"
#include "World.h"
#include "Player.h"
#include "HUD.h"
#include "Database.h"
#include <string>

using namespace sf;

// ─── PlayState (state 4) ─────────────────────────────────────────────────────
// Главный игровой экран: мир, персонаж, камера, HUD.
// Создаётся при нажатии «Играть» и уничтожается при выходе.

class PlayState : public State {
private:
    World   world;
    Player  player;
    HUD     hud;
    View    camera;       // SFML View — следит за персонажем

    Database* db;
    std::string username; // для сохранения рекорда

    // ─── Внутренние методы ────────────────────────────────────────────────────
    void updateCamera(RenderWindow& window);

public:
    // db и username нужны для сохранения рекорда при выходе.
    PlayState(Database* db, const std::string& username);

    // Возвращает:  0 = назад в GameMenu,  -1 = продолжаем
    int update(RenderWindow& window, Event& event) override;

    // Вызывается каждый кадр вне цикла событий (физика, анимация, камера).
    void updateLogic(RenderWindow& window, float dt);

    void render(RenderWindow& window) override;
};