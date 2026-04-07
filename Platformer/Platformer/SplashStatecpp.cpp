#include "SplashState.h"
#include "MainMenuState.h"
#include "Game.h"

SplashState::SplashState() {
    // Используем тот же шрифт для заставки
    font.loadFromFile("assets/font.ttf");
    text.setFont(font);
    text.setString("MY GAME PROJECT");
    text.setCharacterSize(80);

    // Центрируем текст на любом экране
    FloatRect bounds = text.getLocalBounds();
    text.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
    text.setPosition(VideoMode::getDesktopMode().width / 2.0f, VideoMode::getDesktopMode().height / 2.0f);

    alpha = 0; // Начальная прозрачность (черный экран)
    text.setFillColor(Color(255, 255, 255, alpha));
}

void SplashState::update(RenderWindow& window) {
    // Та самая логика из твоего старого кода
    if (alpha < 255) {
        alpha += 2; // Плавное появление
        if (alpha > 255) alpha = 255;
    }
    text.setFillColor(Color(255, 255, 255, alpha));

    // Если прошло время или нажали пробел — идем в меню
    if (timer.getElapsedTime().asSeconds() > 3.0f) {
        // Здесь мы меняем состояние на Главное меню (реализуем в Game чуть позже)
    }
}

void SplashState::render(RenderWindow& window) {
    window.draw(text);
}