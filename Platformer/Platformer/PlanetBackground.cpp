#include "PlanetBackground.h"
#include <iostream>
#include <iomanip>
#include <sstream>

bool PlanetBackground::load(int idx) {
    frames.clear();
    frames.resize(FRAME_COUNT);

    std::string folder = "assets/planets/ezgif-split (" + std::to_string(idx) + ")/";

    for (int i = 0; i < FRAME_COUNT; ++i) {
        // Форматируем номер как два знака: 00, 01 … 59
        std::ostringstream ss;
        ss << std::setfill('0') << std::setw(2) << i;
        std::string path = folder + "frame_" + ss.str() + "_delay-0.07s.gif";

        if (!frames[i].loadFromFile(path)) {
            std::cerr << "PlanetBackground: not found: " << path << "\n";
            loaded = false;
            return false;
        }
        frames[i].setSmooth(true);
    }

    sprite.setTexture(frames[0]);
    sprite.setScale(DISPLAY_SCALE, DISPLAY_SCALE);
    // Центр спрайта в его локальных координатах
    sprite.setOrigin(
        frames[0].getSize().x / 2.f,
        frames[0].getSize().y / 2.f
    );

    currentFrame = 0;
    clock.restart();
    loaded = true;
    return true;
}

void PlanetBackground::advance() {
    if (!loaded) return;
    float elapsed = clock.getElapsedTime().asSeconds();
    if (elapsed >= FRAME_DUR) {
        clock.restart();
        currentFrame = (currentFrame + 1) % FRAME_COUNT;
        sprite.setTexture(frames[currentFrame]);
    }
}

void PlanetBackground::render(RenderWindow& window) {
    if (!loaded) return;

    advance();

    // Рисуем в экранных координатах — сохраняем вид и переключаем на дефолт
    View saved = window.getView();
    window.setView(window.getDefaultView());

    // Центр экрана
    Vector2u sz = window.getSize();
    sprite.setPosition(sz.x / 2.f, sz.y / 2.f);

    // ── Эффект размытия: рисуем несколько смещённых копий с низкой прозрачностью
    sprite.setColor(Color(60, 60, 60, 40));  // тёмный + прозрачный
    float blurOffset = 6.f;
    Vector2f offsets[] = {
        {-blurOffset, 0}, {blurOffset, 0},
        {0, -blurOffset}, {0,  blurOffset},
        {-blurOffset, -blurOffset}, {blurOffset, blurOffset}
    };
    for (auto& off : offsets) {
        sprite.setPosition(sz.x / 2.f + off.x, sz.y / 2.f + off.y);
        window.draw(sprite);
    }

    // ── Основной спрайт — тёмный ──────────────────────────────────────────────
    sprite.setPosition(sz.x / 2.f, sz.y / 2.f);
    sprite.setColor(Color(200, 200, 200, 100));
    window.draw(sprite);
    window.setView(saved);
}