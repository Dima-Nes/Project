#include "SplashState.h"

SplashState::SplashState() {
    backgroundTex.loadFromFile("assets/background.png");
    backgroundSprite.setTexture(backgroundTex);

    float screenX = VideoMode::getDesktopMode().width;
    float screenY = VideoMode::getDesktopMode().height;
    backgroundSprite.setScale(
        screenX / backgroundSprite.getLocalBounds().width,
        screenY / backgroundSprite.getLocalBounds().height
    );

    alpha = 0;
    backgroundSprite.setColor(Color(255, 255, 255, alpha));
}

int SplashState::update(RenderWindow& window, Event& event) {   
    if (alpha < 255) {
        alpha += 1.6;
        if (alpha > 255) alpha = 255;
        backgroundSprite.setColor(Color(255, 255, 255, alpha));
    }
    if (isFinished()) {
        return 1; // Сигнал для Game: "Пора переключаться в Главное меню"
    }

    return 0;
}

void SplashState::render(RenderWindow& window) {
    window.draw(backgroundSprite);
}

bool SplashState::isFinished() {
    return timer.getElapsedTime().asSeconds() > 0.3f/*4.0f - изначально*/;
}