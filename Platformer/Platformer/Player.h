#pragma once
#include <SFML/Graphics.hpp>
#include "Animation.h"
#include "World.h"
#include <string>

using namespace sf;

class Player {
public:
    // ─── Параметры спрайт-листа knight.png ────────────────────────────────────
    // Если анимация выглядит неправильно — измени эти константы.
    // Открой knight.png в редакторе и измерь ширину/высоту одного кадра.
    static constexpr int FRAME_W = 120;  // ширина кадра, пикселей
    static constexpr int FRAME_H = 80;  // высота кадра, пикселей
    static constexpr int IDLE_ROW = 0;  // строка IDLE в спрайт-листе
    static constexpr int IDLE_FRAMES = 10;  // кол-во кадров IDLE
    static constexpr int RUN_ROW = 1;  // строка RUN
    static constexpr int RUN_FRAMES = 10;  // кол-во кадров RUN

    // ─── Физика ───────────────────────────────────────────────────────────────
    static constexpr float GRAVITY = 1800.f;  // ускорение свободного падения, px/s²
    static constexpr float MOVE_SPEED = 210.f;  // скорость бега, px/s
    static constexpr float JUMP_SPEED = 600.f;  // начальная скорость прыжка (вверх), px/s

    // ─── Хитбокс ──────────────────────────────────────────────────────────────
    // Хитбокс меньше спрайта — так коллизии выглядят естественнее.
    static constexpr float HITBOX_W = 36.f;  // px
    static constexpr float HITBOX_H = 56.f;  // px

    // ─── Масштаб спрайта ──────────────────────────────────────────────────────
    // Спрайт 120×80 * 0.55 ≈ 66×44 видимых пикселей — чуть крупнее хитбокса.
    // Увеличь, если персонаж кажется маленьким.
    static constexpr float SPRITE_SCALE = 0.55f;

private:
    // Спрайт и анимация
    Texture  tex;
    Sprite   sprite;
    Animator animator;
    AnimClip clipIdle, clipRun;

    // Физика
    Vector2f pos;         // левый верхний угол хитбокса в мировых пикселях
    Vector2f vel;         // скорость, px/s
    bool     onGround = false;

    // Характеристики игрока
    int score = 0;
    int lives = 3;

    // ─── Внутренние методы ────────────────────────────────────────────────────
    void buildClips();

    // Двигает по одной оси и разрешает коллизии с миром.
    void moveX(const World& world, float dx);
    void moveY(const World& world, float dy);

public:
    Player();

    // Загрузить текстуру. Возвращает false при ошибке.
    bool loadTexture(const std::string& path);

    // Поставить персонажа на поверхность в центре мира.
    void spawn(const World& world);

    // Обрабатывать одиночные события (прыжок).
    void handleEvent(const Event& event);

    // Обновлять физику и анимацию каждый кадр.
    void update(float dt, const World& world);

    // Нарисовать персонажа.
    void render(RenderWindow& window);

    // ─── Геттеры ──────────────────────────────────────────────────────────────
    Vector2f  getCenter()  const { return pos + Vector2f(HITBOX_W / 2.f, HITBOX_H / 2.f); }
    FloatRect getHitbox()  const { return { pos.x, pos.y, HITBOX_W, HITBOX_H }; }
    int       getScore()   const { return score; }
    int       getLives()   const { return lives; }

    void addScore(int v) { score += v; }
    void loseLife() { if (lives > 0) --lives; }
};