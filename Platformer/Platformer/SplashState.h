#pragma once
#include "State.h"

class SplashState : public State {
private:
    Texture backgroundTex;
    Sprite backgroundSprite;
    Clock timer;
    int alpha;

public:
    SplashState();
    void update(RenderWindow& window) override;
    void render(RenderWindow& window) override;
    bool isFinished(); // Метод для проверки окончания заставки
};