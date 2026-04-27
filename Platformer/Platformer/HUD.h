#pragma once
#include <SFML/Graphics.hpp>
#include "Player.h"

using namespace sf;

// ─── HUD ──────────────────────────────────────────────────────────────────────
// Рисуется поверх мира в экранных координатах (не мировых).
// Всегда видна, даже когда камера движется.

class HUD {
private:
    Font font;
    Text txtScore;
    Text txtLives;

    void setupText(Text& t, float x, float y, Color color);

public:
    HUD();
    bool loadFont(const std::string& path);

    // Обновить текст из данных игрока.
    void update(const Player& player);

    // Рисует в экранных координатах (переключает View внутри).
    void render(RenderWindow& window);
};