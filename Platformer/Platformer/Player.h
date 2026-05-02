#pragma once
#include <SFML/Graphics.hpp>
#include "Animation.h"
#include "World.h"
#include <string>

using namespace sf;

class Player {
public:
    // ─── Параметры спрайт-листа adventurer.png ────────────────────────────────
    static constexpr int FRAME_W = 50;
    static constexpr int FRAME_H = 38;

    static constexpr int IDLE_ROW = 0;
    static constexpr int IDLE_FRAMES = 4;
    static constexpr int IDLE_COL = 0;   // ← старт с 1-й колонки (0-based)

    static constexpr int RUN_ROW = 1;
    static constexpr int RUN_FRAMES = 6;
    static constexpr int RUN_COL = 1;   // ← run начинается со 2-й колонки

    // ─── Физика ───────────────────────────────────────────────────────────────
    static constexpr float GRAVITY = 1800.f;
    static constexpr float MOVE_SPEED = 210.f;
    static constexpr float JUMP_SPEED = 600.f;
    static constexpr float ATTACK_SLOWDOWN = 0.15f; // множитель скорости при атаке

    static constexpr float COYOTE_TIME = 0.10f;

    static constexpr float HITBOX_W = 30.f;
    static constexpr float HITBOX_H = 44.f;
    static constexpr float SPRITE_SCALE = 2.0f;

private:
    // Спрайт и анимация
    Texture  tex;
    Sprite   sprite;
    Animator animator;
    AnimClip clipIdle, clipRun;

    // Физика
    Vector2f pos;               // левый верхний угол хитбокса в мировых пикселях
    Vector2f vel;               // скорость, px/s
    bool     onGround = false;
    float    coyoteTimer = 0.f; // секунды, в течение которых можно прыгнуть после края

    // Характеристики игрока
    int score = 0;
    int lives = 3;

    // ─── Внутренние методы ────────────────────────────────────────────────────
    void buildClips();
    void moveX(const World& world, float dx);
    void moveY(const World& world, float dy);

    AnimClip clipAttack;
    bool     isAttacking = false;

public:
    Player();

    bool loadTexture(const std::string& path);
    void spawn(const World& world);
    void spawnAt(float x, float y);
    void handleEvent(const Event& event);
    void update(float dt, const World& world);
    void render(RenderWindow& window);

    // ─── Геттеры ──────────────────────────────────────────────────────────────
    Vector2f  getCenter()  const { return pos + Vector2f(HITBOX_W / 2.f, HITBOX_H / 2.f); }
    FloatRect getHitbox()  const { return { pos.x, pos.y, HITBOX_W, HITBOX_H }; }
    int       getScore()   const { return score; }
    int       getLives()   const { return lives; }

    void addScore(int v) { score += v; }
    void loseLife() { if (lives > 0) --lives; }
};