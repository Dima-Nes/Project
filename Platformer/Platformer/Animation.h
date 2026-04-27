#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

using namespace sf;

// ─── AnimClip ─────────────────────────────────────────────────────────────────
// Описывает одну анимацию: набор прямоугольников из спрайт-листа.
// Создаётся один раз в конструкторе объекта, потом только передаётся в Animator.

struct AnimClip {
    std::string          name;
    std::vector<IntRect> frames;
    float frameDuration = 0.1f;  // секунд на кадр
    bool  loop = true;
};

// ─── Animator ─────────────────────────────────────────────────────────────────
// Проигрывает AnimClip: считает время, переключает кадры, поддерживает зеркало.
// Не зависит от Sprite — просто возвращает текущий IntRect.

class Animator {
private:
    const AnimClip* current = nullptr;
    int             frameIdx = 0;
    float           elapsed = 0.f;
    bool            flipped = false;

public:
    // Начать анимацию. Если та же — не сбрасывать (если forceRestart = false).
    void play(const AnimClip& clip, bool forceRestart = false);

    // Вызывать каждый кадр с delta-time.
    void update(float dt);

    // Текущий прямоугольник для Sprite::setTextureRect().
    IntRect getCurrentRect() const;

    // Закончилась ли неповторяющаяся анимация?
    bool isFinished() const;

    void setFlipped(bool f) { flipped = f; }
    bool isFlipped()  const { return flipped; }
};