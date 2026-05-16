#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

using namespace sf;

// ─── PlanetBackground ────────────────────────────────────────────────────────
// Загружает 60 кадров планеты и воспроизводит их по центру экрана.
// Используется во всех меню. Создаётся один раз в Game, передаётся по указателю.

class PlanetBackground {
public:
    static constexpr int   FRAME_COUNT = 60;
    static constexpr float DISPLAY_SCALE = 9.0f;   // 200px → 1000px
    static constexpr float FRAME_DUR = 0.15f;  // 9 сек на цикл

    PlanetBackground() = default;

    // idx = 0..8  →  папка "assets/planets/ezgif-split (N)"
    bool load(int idx);

    // Вызывать в render() каждого меню — обновляет и рисует
    void render(RenderWindow& window);

private:
    std::vector<Texture> frames;
    Sprite               sprite;
    int                  currentFrame = 0;
    Clock                clock;
    bool                 loaded = false;

    void advance();   // переключает кадр по таймеру
};