#include "Player.h"
#include <cmath>
#include <algorithm>

// ─── Конструктор ─────────────────────────────────────────────────────────────

Player::Player() {
    buildClips();
    sprite.setOrigin(FRAME_W / 2.f, FRAME_H / 2.f);
    sprite.setScale(SPRITE_SCALE, SPRITE_SCALE);
}

void Player::buildClips() {
    // ── Idle ──────────────────────────────────────────────────────────────
    clipIdle.name = "idle";
    clipIdle.frameDuration = 0.12f;
    clipIdle.loop = true;
    for (int i = 0; i < IDLE_FRAMES; ++i)
        clipIdle.frames.push_back({
            (IDLE_COL + i) * FRAME_W,
            IDLE_ROW * FRAME_H,
            FRAME_W, FRAME_H
            });

    // ── Run ───────────────────────────────────────────────────────────────
    clipRun.name = "run";
    clipRun.frameDuration = 0.08f;
    clipRun.loop = true;
    for (int i = 0; i < RUN_FRAMES; ++i)
        clipRun.frames.push_back({
            (RUN_COL + i) * FRAME_W,
            RUN_ROW * FRAME_H,
            FRAME_W, FRAME_H
            });

    // ── Attack ────────────────────────────────────────────────────────────
    // Кадры разбиты на две строки: [13][4-6] и [14][0-1] (0-based)
    clipAttack.name = "attack";
    clipAttack.frameDuration = 0.09f;
    clipAttack.loop = false;
    //// Строка 13, колонки 4,5,6
    //clipAttack.frames.push_back({ 4 * FRAME_W, 13 * FRAME_H, FRAME_W, FRAME_H });
    //clipAttack.frames.push_back({ 5 * FRAME_W, 13 * FRAME_H, FRAME_W, FRAME_H });
    //clipAttack.frames.push_back({ 6 * FRAME_W, 13 * FRAME_H, FRAME_W, FRAME_H });
    //// Строка 14, колонки 0,1
    //clipAttack.frames.push_back({ 0 * FRAME_W, 14 * FRAME_H, FRAME_W, FRAME_H });
    //clipAttack.frames.push_back({ 1 * FRAME_W, 14 * FRAME_H, FRAME_W, FRAME_H });
    // Атака — явные координаты от измеренной точки (215, 492)
    clipAttack.frames.push_back({ 200,        480, FRAME_W, FRAME_H });
    clipAttack.frames.push_back({ 200 + 50,   480, FRAME_W, FRAME_H });
    clipAttack.frames.push_back({ 200 + 100,  480, FRAME_W, FRAME_H });
    clipAttack.frames.push_back({ 200,         480 + FRAME_H, FRAME_W, FRAME_H });
    clipAttack.frames.push_back({ 200 + 50,    480 + FRAME_H, FRAME_W, FRAME_H });
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
    coyoteTimer = 0.f;
    animator.play(clipIdle);
}

void Player::spawnAt(float x, float y) {
    pos = { x - HITBOX_W / 2.f, y - HITBOX_H / 2.f };
    vel = { 0.f, 0.f };
    onGround = false;
    coyoteTimer = 0.f;
    animator.play(clipIdle);
}

// ─── Прыжок ──────────────────────────────────────────────────────────────────

void Player::handleEvent(const Event& event) {
    if (event.type == Event::KeyPressed) {
        bool jumpKey = event.key.code == Keyboard::Space
            || event.key.code == Keyboard::W
            || event.key.code == Keyboard::Up;
        if (jumpKey && (onGround || coyoteTimer > 0.f)) {
            vel.y = -JUMP_SPEED;
            onGround = false;
            coyoteTimer = 0.f;
        }
    }

    // ── Атака по ЛКМ ──────────────────────────────────────────────────────
    if (event.type == Event::MouseButtonPressed
        && event.mouseButton.button == Mouse::Left
        && !isAttacking)            // нельзя прервать атаку новой
    {
        isAttacking = true;
        animator.play(clipAttack, true);
    }
}

// ─── Коллизии ────────────────────────────────────────────────────────────────

void Player::moveX(const World& world, float dx) {
    pos.x += dx;

    float l = pos.x;
    float r = pos.x + HITBOX_W - 1.f;
    float t = pos.y + 4.f;
    float b = pos.y + HITBOX_H - 4.f;

    if (dx > 0.f) {
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

    const float minX = 2.f * World::TILE_SIZE;
    const float maxX = (World::WIDTH - 2.f) * World::TILE_SIZE - HITBOX_W;
    if (pos.x < minX) { pos.x = minX; vel.x = 0.f; }
    if (pos.x > maxX) { pos.x = maxX; vel.x = 0.f; }
}

void Player::moveY(const World& world, float dy) {
    pos.y += dy;

    float l = pos.x + 2.f;
    float r = pos.x + HITBOX_W - 2.f;
    float t = pos.y;
    float b = pos.y + HITBOX_H - 1.f;

    if (dy > 0.f) {
        if (world.isSolidAt(l, b) || world.isSolidAt(r, b)) {
            pos.y = std::floor(b / World::TILE_SIZE) * World::TILE_SIZE - HITBOX_H;
            vel.y = 0.f;
            onGround = true;
        }
    }
    else if (dy < 0.f) {
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

    // 2. Гравитация
    if (!onGround)
        vel.y += GRAVITY * dt;
    vel.y = std::min(vel.y, 1200.f);

    // 3. Сбрасываем флаг перед движением
    bool wasOnGround = onGround;
    onGround = false;

    // 4. Коллизии с тайлами
    this->moveX(world, vel.x * dt);
    this->moveY(world, vel.y * dt);

    // ── ИСПРАВЛЕНИЕ: Ground probe ────────────────────────────────────────────
    // moveY устанавливает onGround только когда vel.y > 0.
    // Когда игрок стоит на месте (vel.y == 0), dy=0 и onGround не обновляется.
    // Это приводит к чередованию true/false каждый кадр → прыжок работает
    // через раз. Ground probe проверяет 1px ниже хитбокса и исправляет это.
    {
        float l = pos.x + 2.f;
        float r = pos.x + HITBOX_W - 2.f;
        if (world.isSolidAt(l, pos.y + HITBOX_H + 1.f) ||
            world.isSolidAt(r, pos.y + HITBOX_H + 1.f))
            onGround = true;
    }

    // ── Coyote time ──────────────────────────────────────────────────────────
    // Позволяет прыгнуть ещё ~100ms после схода с платформы.
    if (wasOnGround && !onGround && vel.y > 0.f)
        coyoteTimer = COYOTE_TIME;  // начинаем отсчёт

    if (coyoteTimer > 0.f)
        coyoteTimer -= dt;

    // 5. ── ЖЁСТКИЕ ГРАНИЦЫ МИРА ──────────────────────────────────────────────
    const float worldMinX = 2.f * World::TILE_SIZE;
    const float worldMaxX = (World::WIDTH - 2.f) * World::TILE_SIZE - HITBOX_W;
    const float worldMinY = 0.f;
    // Используем дно хитбокса для проверки нижней границы
    const float worldMaxY = (float)(World::HEIGHT * World::TILE_SIZE);

    if (pos.x < worldMinX) { pos.x = worldMinX; vel.x = 0.f; }
    if (pos.x > worldMaxX) { pos.x = worldMaxX; vel.x = 0.f; }
    if (pos.y < worldMinY) { pos.y = worldMinY; vel.y = 0.f; }
    if (pos.y + HITBOX_H > worldMaxY) { spawn(world); return; }  // упал за низ → возрождение

    // 6. Анимация
    if (isAttacking) {
        // Замедляем персонажа во время атаки
        vel.x *= ATTACK_SLOWDOWN;

        animator.update(dt);

        if (animator.isFinished()) {
            isAttacking = false;
            animator.play(clipIdle, true);
        }
    }
    else {
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
}

// ─── Рендер ──────────────────────────────────────────────────────────────────

void Player::render(RenderWindow& window) {
    float cx = pos.x + HITBOX_W / 2.f;
    float cy = pos.y + HITBOX_H / 2.f;

    sprite.setTextureRect(animator.getCurrentRect());
    sprite.setPosition(cx, cy);

    float sx = animator.isFlipped() ? -SPRITE_SCALE : SPRITE_SCALE;
    sprite.setScale(sx, SPRITE_SCALE);

    window.draw(sprite);
}