#include "Player.h"
#include <cmath>
#include <algorithm>

// ─── Конструктор ─────────────────────────────────────────────────────────────

Player::Player() {
    buildClips();
    // Точка привязки спрайта — его центр
    sprite.setOrigin(FRAME_W / 2.f, FRAME_H / 2.f);
    sprite.setScale(SPRITE_SCALE, SPRITE_SCALE);
}

void Player::buildClips() {
    // IDLE — строка 0
    clipIdle.name = "idle";
    clipIdle.frameDuration = 0.10f;
    clipIdle.loop = true;
    for (int i = 0; i < IDLE_FRAMES; ++i)
        clipIdle.frames.push_back({ i * FRAME_W, IDLE_ROW * FRAME_H, FRAME_W, FRAME_H });

    // RUN — строка 1
    clipRun.name = "run";
    clipRun.frameDuration = 0.075f;
    clipRun.loop = true;
    for (int i = 0; i < RUN_FRAMES; ++i)
        clipRun.frames.push_back({ i * FRAME_W, RUN_ROW * FRAME_H, FRAME_W, FRAME_H });
}

bool Player::loadTexture(const std::string& path) {
    if (!tex.loadFromFile(path)) return false;
    sprite.setTexture(tex);
    return true;
}

// ─── Спавн ───────────────────────────────────────────────────────────────────

void Player::spawn(const World& world) {
    int midX = World::WIDTH / 2;
    float surfY = world.surfacePixelY(midX);
    pos = {
        (float)(midX * World::TILE_SIZE) - HITBOX_W / 2.f,
        surfY - HITBOX_H
    };
    vel = { 0.f, 0.f };
    onGround = false;
    animator.play(clipIdle);
}

// ─── События (прыжок) ─────────────────────────────────────────────────────────

void Player::handleEvent(const Event& event) {
    if (event.type == Event::KeyPressed) {
        bool jumpKey = event.key.code == Keyboard::Space
            || event.key.code == Keyboard::W
            || event.key.code == Keyboard::Up;
        if (jumpKey && onGround) {
            vel.y = -JUMP_SPEED;
            onGround = false;
        }
    }
}

// ─── Коллизии ────────────────────────────────────────────────────────────────
// Разделённые оси: сначала X, потом Y. Простая AABB.

void Player::moveX(const World& world, float dx) {
    pos.x += dx;

    float l = pos.x;
    float r = pos.x + HITBOX_W - 1.f;
    float t = pos.y + 4.f;              // немного отступаем от углов
    float b = pos.y + HITBOX_H - 4.f;

    if (dx > 0.f) {
        // Проверяем правый край на двух высотах
        if (world.isSolidAt(r, t) || world.isSolidAt(r, b)) {
            pos.x = std::floor(r / World::TILE_SIZE) * World::TILE_SIZE - HITBOX_W;
            vel.x = 0.f;
        }
    }
    else if (dx < 0.f) {
        if (world.isSolidAt(l, t) || world.isSolidAt(l, b)) {
            pos.x = std::ceil(l / World::TILE_SIZE) * World::TILE_SIZE;
            vel.x = 0.f;
        }
    }
}

void Player::moveY(const World& world, float dy) {
    pos.y += dy;

    float l = pos.x + 2.f;
    float r = pos.x + HITBOX_W - 2.f;
    float t = pos.y;
    float b = pos.y + HITBOX_H - 1.f;

    if (dy > 0.f) {
        // Падаем вниз — проверяем нижний край
        if (world.isSolidAt(l, b) || world.isSolidAt(r, b)) {
            pos.y = std::floor(b / World::TILE_SIZE) * World::TILE_SIZE - HITBOX_H;
            vel.y = 0.f;
            onGround = true;
        }
        else {
            onGround = false;
        }
    }
    else if (dy < 0.f) {
        // Прыжок вверх — проверяем верхний край (потолок)
        if (world.isSolidAt(l, t) || world.isSolidAt(r, t)) {
            pos.y = std::ceil(t / World::TILE_SIZE) * World::TILE_SIZE;
            vel.y = 0.f;
        }
    }
}

// ─── Обновление ──────────────────────────────────────────────────────────────

void Player::update(float dt, const World& world) {
    // 1. Горизонтальный ввод
    float moveX = 0.f;
    if (Keyboard::isKeyPressed(Keyboard::A) || Keyboard::isKeyPressed(Keyboard::Left))
        moveX -= 1.f;
    if (Keyboard::isKeyPressed(Keyboard::D) || Keyboard::isKeyPressed(Keyboard::Right))
        moveX += 1.f;

    vel.x = moveX * MOVE_SPEED;

    // 2. Гравитация (только в воздухе)
    if (!onGround)
        vel.y += GRAVITY * dt;

    // Ограничиваем скорость падения
    vel.y = std::min(vel.y, 1200.f);

    // 3. Коллизии (разделённые оси)
    onGround = false;           // сбрасываем — moveY восстановит если стоим
    this->moveX(world, vel.x * dt);
    this->moveY(world, vel.y * dt);

    // 4. Анимация
    bool running = std::abs(vel.x) > 5.f;
    if (running) {
        animator.play(clipRun);
        animator.setFlipped(vel.x < 0.f);
    }
    else {
        animator.play(clipIdle);
    }
    animator.update(dt);
}

// ─── Рендер ──────────────────────────────────────────────────────────────────

void Player::render(RenderWindow& window) {
    // Центрируем спрайт над центром хитбокса
    float cx = pos.x + HITBOX_W / 2.f;
    float cy = pos.y + HITBOX_H / 2.f;

    sprite.setTextureRect(animator.getCurrentRect());
    sprite.setPosition(cx, cy);

    float sx = animator.isFlipped() ? -SPRITE_SCALE : SPRITE_SCALE;
    sprite.setScale(sx, SPRITE_SCALE);

    window.draw(sprite);
}