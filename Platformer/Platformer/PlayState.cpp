#include "PlayState.h"
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <iostream>

// ─── Конструктор ─────────────────────────────────────────────────────────────

PlayState::PlayState(Database* db, const std::string& username)
    : db(db), username(username)
{
    // Генерация мира. Позже можно передавать конкретный сид из меню.
    int seed = (int)std::time(nullptr);
    std::cout << "Generating world with seed: " << seed << std::endl;
    world.generate(seed);

    // Загрузка персонажа
    // Если текстура не найдена — персонаж отрисуется пустым спрайтом,
    // но физика и коллизии будут работать.
    if (!player.loadTexture("assets/brackeys_platformer_assets/sprites/knight.png")) {
        std::cerr << "Warning: knight.png not found. Check path." << std::endl;
    }
    player.spawn(world);

    // HUD
    if (!hud.loadFont("assets/font.ttf")) {
        std::cerr << "Warning: font.ttf not found." << std::endl;
    }
    hud.update(player);
}

// ─── Камера ──────────────────────────────────────────────────────────────────
// Плавно следит за персонажем, не выходя за границы мира.

void PlayState::updateCamera(RenderWindow& window) {
    float ww = (float)window.getSize().x;
    float wh = (float)window.getSize().y;

    // Инициализируем View по размеру окна если ещё не делали
    if (camera.getSize().x == 0) {
        camera.setSize(ww, wh);
        camera.setCenter(player.getCenter());
    }

    float worldW = World::WIDTH * World::TILE_SIZE;
    float worldH = World::HEIGHT * World::TILE_SIZE;

    Vector2f target = player.getCenter();

    // Ограничиваем камеру границами мира
    float hw = ww / 2.f;
    float hh = wh / 2.f;
    target.x = std::max(hw, std::min(target.x, worldW - hw));
    target.y = std::max(hh, std::min(target.y, worldH - hh));

    // Плавная интерполяция (lerp) для мягкого движения камеры
    Vector2f current = camera.getCenter();
    float    lerpK = 0.12f;                     // 0 = статично, 1 = мгновенно
    camera.setCenter(current + (target - current) * lerpK);

    window.setView(camera);
}

// ─── update (события) ────────────────────────────────────────────────────────

int PlayState::update(RenderWindow& window, Event& event) {
    // ESC → сохраняем рекорд и возвращаемся в GameMenu
    if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape) {
        if (db) db->updateHighScore(username, player.getScore());
        return 0;
    }

    player.handleEvent(event);
    return -1;
}

// ─── updateLogic (каждый кадр) ───────────────────────────────────────────────

void PlayState::updateLogic(RenderWindow& window, float dt) {
    player.update(dt, world);
    hud.update(player);
    updateCamera(window);
}

// ─── render ──────────────────────────────────────────────────────────────────

void PlayState::render(RenderWindow& window) {
    world.render(window, camera);   // тайлы
    player.render(window);          // персонаж
    hud.render(window);             // HUD в экранных координатах
}