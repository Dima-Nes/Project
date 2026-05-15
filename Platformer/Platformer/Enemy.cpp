#include "Enemy.h"
#include <cmath>
#include <algorithm>
#include <cstdlib>

Enemy::Enemy(EnemyType t, float worldX, float worldY) : type(t) {
    buildClips();
    sprite.setOrigin(FRAME_W / 2.f, FRAME_H / 2.f);
    pos = { worldX - HITBOX_W / 2.f, worldY - HITBOX_H };
    vel = { 0.f, 0.f };
    patrolDir = (std::rand() % 2 == 0) ? 1.f : -1.f;
    patrolTimer = (float)(std::rand() % 3);
    animator.play(clipIdle);
}

void Enemy::buildClips() {
    clipIdle.name = "idle"; clipIdle.frameDuration = 0.14f; clipIdle.loop = true;
    for (int i = 0; i < IDLE_FRAMES; ++i)
        clipIdle.frames.push_back({ i * FRAME_W, 0 * FRAME_H, FRAME_W, FRAME_H });

    clipWalk.name = "walk"; clipWalk.frameDuration = 0.09f; clipWalk.loop = true;
    for (int i = 0; i < WALK_FRAMES; ++i)
        clipWalk.frames.push_back({ i * FRAME_W, 1 * FRAME_H, FRAME_W, FRAME_H });

    clipAttack.name = "attack"; clipAttack.frameDuration = 0.10f; clipAttack.loop = false;
    for (int i = 0; i < ATK_FRAMES; ++i)
        clipAttack.frames.push_back({ i * FRAME_W, 2 * FRAME_H, FRAME_W, FRAME_H });
}

bool Enemy::loadTexture(const std::string& path) {
    if (!tex.loadFromFile(path)) return false;
    sprite.setTexture(tex);
    return true;
}

// ─── Коллизии ────────────────────────────────────────────────────────────────

void Enemy::moveX(const World& world, float dx) {
    blockedX = false;   // сбрасываем перед движением
    pos.x += dx;
    float l = pos.x, r = pos.x + HITBOX_W - 1.f;
    float t = pos.y + 4.f, b = pos.y + HITBOX_H - 4.f;

    if (dx > 0.f && (world.isSolidAt(r, t) || world.isSolidAt(r, b))) {
        pos.x = std::floor(r / World::TILE_SIZE) * World::TILE_SIZE - HITBOX_W;
        vel.x = 0.f;
        blockedX = true;
        if (state != EnemyState::Chase) patrolDir = -1.f;
    }
    else if (dx < 0.f && (world.isSolidAt(l, t) || world.isSolidAt(l, b))) {
        pos.x = std::ceil(l / World::TILE_SIZE) * World::TILE_SIZE;
        vel.x = 0.f;
        blockedX = true;
        if (state != EnemyState::Chase) patrolDir = 1.f;
    }

    const float minX = 2.f * World::TILE_SIZE;
    const float maxX = (World::WIDTH - 2.f) * World::TILE_SIZE - HITBOX_W;
    if (pos.x < minX) { pos.x = minX; patrolDir = 1.f; }
    if (pos.x > maxX) { pos.x = maxX; patrolDir = -1.f; }
}

void Enemy::moveY(const World& world, float dy) {
    pos.y += dy;
    float l = pos.x + 2.f, r = pos.x + HITBOX_W - 2.f;

    if (dy > 0.f && (world.isSolidAt(l, pos.y + HITBOX_H - 1.f) ||
        world.isSolidAt(r, pos.y + HITBOX_H - 1.f))) {
        pos.y = std::floor((pos.y + HITBOX_H - 1.f) / World::TILE_SIZE)
            * World::TILE_SIZE - HITBOX_H;
        vel.y = 0.f;
        onGround = true;
    }
    else if (dy < 0.f && (world.isSolidAt(l, pos.y) || world.isSolidAt(r, pos.y))) {
        pos.y = std::ceil(pos.y / World::TILE_SIZE) * World::TILE_SIZE;
        vel.y = 0.f;
    }
}

// ─── AI ──────────────────────────────────────────────────────────────────────

void Enemy::updateAI(float dt, Vector2f playerCenter) {
    Vector2f center = getCenter();
    float dist = std::hypot(playerCenter.x - center.x, playerCenter.y - center.y);

    attackTimer = std::max(0.f, attackTimer - dt);
    patrolTimer = std::max(0.f, patrolTimer - dt);

    switch (state) {

    case EnemyState::Patrol:
        vel.x = patrolDir * MOVE_SPEED;
        if (patrolTimer <= 0.f) {
            patrolDir = -patrolDir;
            patrolTimer = PATROL_TURN + (std::rand() % 20) / 10.f;
        }
        if (dist < DETECT_RADIUS)
            state = EnemyState::Chase;
        break;

    case EnemyState::Chase:
        if (dist > DETECT_RADIUS * 1.3f) {
            state = EnemyState::Patrol; break;
        }
        if (dist < ATTACK_RADIUS && attackTimer <= 0.f) {
            state = EnemyState::Attack;
            hitting = false;
            animator.play(clipAttack, true);
            break;
        }
        vel.x = (playerCenter.x > center.x ? 1.f : -1.f) * MOVE_SPEED;
        break;

    case EnemyState::Attack:
        // Плавно тормозим вместо резкой остановки
        vel.x *= 0.85f;

        if (!hitting && animator.getCurrentRect().left == 3 * FRAME_W)
            hitting = true;

        if (animator.isFinished()) {
            attackTimer = ATTACK_COOLDOWN;
            hitting = false;
            state = (dist < DETECT_RADIUS)
                ? EnemyState::Chase : EnemyState::Patrol;
        }
        break;

    case EnemyState::Hit:
        vel.x *= 0.80f;
        hitTimer -= dt;
        if (hitTimer <= 0.f)
            state = (dist < DETECT_RADIUS) ? EnemyState::Chase : EnemyState::Patrol;
        break;

    case EnemyState::Dead:
        vel.x = 0.f;
        break;
    }

    if (vel.x < -1.f) facingLeft = true;
    else if (vel.x > 1.f) facingLeft = false;
}

// ─── update ──────────────────────────────────────────────────────────────────

void Enemy::update(float dt, const World& world, Vector2f playerCenter) {
    // Таймер неуязвимости
    if (invincibleTimer > 0.f) invincibleTimer -= dt;

    if (isDead()) return;

    updateAI(dt, playerCenter);

    if (!onGround) vel.y += GRAVITY * dt;
    vel.y = std::min(vel.y, 1200.f);
    onGround = false;

    moveX(world, vel.x * dt);
    moveY(world, vel.y * dt);

    // Ground probe
    float l = pos.x + 2.f, r = pos.x + HITBOX_W - 2.f;
    if (world.isSolidAt(l, pos.y + HITBOX_H + 1.f) ||
        world.isSolidAt(r, pos.y + HITBOX_H + 1.f))
        onGround = true;

    if (blockedX && onGround)
        vel.y = -JUMP_SPEED;

    // ── Patrol: разворот у края платформы ────────────────────────────────
    if (onGround && state == EnemyState::Patrol) {
        float probeX = (patrolDir > 0.f)
            ? pos.x + HITBOX_W + 2.f
            : pos.x - 2.f;
        if (!world.isSolidAt(probeX, pos.y + HITBOX_H + 4.f))
            patrolDir = -patrolDir;   // разворот у края
    }

    // Выпал за мир
    if (pos.y > (float)(World::HEIGHT * World::TILE_SIZE)) hp = 0;

    // Анимация
    if (state == EnemyState::Patrol || state == EnemyState::Chase) {
        if (std::abs(vel.x) > 1.f) animator.play(clipWalk);
        else                        animator.play(clipIdle);
    }
    animator.update(dt);
}

// ─── Урон ────────────────────────────────────────────────────────────────────

void Enemy::takeDamage(int dmg) {
    if (isDead()) return;
    if (invincibleTimer > 0.f) return;   // ← неуязвим

    invincibleTimer = INVINCIBLE_TIME;   // ← включаем

    hp -= dmg;
    if (hp <= 0) { hp = 0; state = EnemyState::Dead; vel = { 0,0 }; return; }

    state = EnemyState::Hit;
    hitTimer = HIT_DURATION;
    vel.x = (facingLeft ? 1.f : -1.f) * 180.f;
    vel.y = -220.f;
    animator.play(clipIdle, true);
}

// ─── Рендер ──────────────────────────────────────────────────────────────────

void Enemy::render(RenderWindow& window) {
    if (isDead()) return;
    float cx = pos.x + HITBOX_W / 2.f;
    float cy = pos.y + HITBOX_H / 2.f;

    sprite.setTextureRect(animator.getCurrentRect());
    sprite.setPosition(cx, cy);

    float sx = facingLeft ? -SPRITE_SCALE_X : SPRITE_SCALE_X;
    sprite.setScale(sx, SPRITE_SCALE_Y);   // ← здесь должен быть SPRITE_SCALE, не 1.f
    sprite.setColor(state == EnemyState::Hit
        ? Color(255, 100, 100) : Color::White);
    window.draw(sprite);
}