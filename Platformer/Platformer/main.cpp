#include <SFML/Graphics.hpp>
#include <iostream>
#include "Database.h"

// Состояния игры
enum GameState {
    SPLASH_SCREEN,
    MAIN_MENU,
    GAMEPLAY
};

// Функция для быстрой центровки текста (чтобы не писать одно и то же 3 раза)
void centerText(sf::Text& text, float x, float y) {
    sf::FloatRect bounds = text.getLocalBounds();
    // Ставим точку отсчета (Origin) в центр текста, а не в левый верхний угол
    text.setOrigin(bounds.width / 2, bounds.height / 2);
    text.setPosition(x, y);
}

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Lost Horizons");

    // ДОБАВЬ ВОТ ЭТУ СТРОКУ:
    window.setFramerateLimit(60);

    // ... дальше идет загрузка шрифтов ...

    // --- ЗАГРУЗКА ---
    sf::Font font;
    if (!font.loadFromFile("assets/font.ttf")) return -1;

    sf::Texture bgTexture;
    if (!bgTexture.loadFromFile("assets/background.png")) return -1;

    // Включаем сглаживание текстуры фона (чтобы при растягивании было красиво)
    bgTexture.setSmooth(true);
    sf::Sprite background(bgTexture);

    sf::Texture splashTexture;
    if (!splashTexture.loadFromFile("assets/background.png")) return -1;
    sf::Sprite splashSprite(splashTexture);

    // --- КНОПКИ ---
    sf::Text titleText("LOST HORIZONS", font, 60);
    titleText.setFillColor(sf::Color::Yellow);
    titleText.setOutlineColor(sf::Color::Black);
    titleText.setOutlineThickness(4);

    sf::Text btnRegister("1. Registration", font, 35);
    btnRegister.setOutlineColor(sf::Color::Black);
    btnRegister.setOutlineThickness(2);

    sf::Text btnLogin("2. Login", font, 35);
    btnLogin.setOutlineColor(sf::Color::Black);
    btnLogin.setOutlineThickness(2);

    sf::Text btnExit("3. Exit", font, 35);
    btnExit.setOutlineColor(sf::Color::Black);
    btnExit.setOutlineThickness(2);

    // --- ПЕРЕМЕННЫЕ ---
    GameState currentState = SPLASH_SCREEN;
    sf::Clock splashTimer;

    // --- ПЕРВАЯ НАСТРОЙКА ---
    // Вызываем "фейковое" событие изменения размера, чтобы всё встало на места при старте
    sf::Vector2u size = window.getSize();
    float centerX = size.x / 2.0f;
    float centerY = size.y / 2.0f;

    // Центруем заставку
    sf::FloatRect splashBounds = splashSprite.getLocalBounds();
    splashSprite.setOrigin(splashBounds.width / 2, splashBounds.height / 2);
    splashSprite.setPosition(centerX, centerY);

    // Центруем меню
    centerText(titleText, centerX, 100);
    centerText(btnRegister, centerX, centerY - 20);
    centerText(btnLogin, centerX, centerY + 50);
    centerText(btnExit, centerX, centerY + 120);

    // Масштабируем фон
    background.setScale(
        (float)size.x / bgTexture.getSize().x,
        (float)size.y / bgTexture.getSize().y
    );


    // --- ГЛАВНЫЙ ЦИКЛ ---
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
                window.close();

            // !!! МАГИЯ РЕСАЙЗА !!!
            if (event.type == sf::Event::Resized) {
                // 1. Обновляем "Камеру" (View), чтобы она видела новую область, а не растягивала старую
                sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                window.setView(sf::View(visibleArea));

                // 2. Пересчитываем центр
                float newCenterX = event.size.width / 2.0f;
                float newCenterY = event.size.height / 2.0f;

                // 3. Растягиваем фон заново
                background.setScale(
                    (float)event.size.width / bgTexture.getSize().x,
                    (float)event.size.height / bgTexture.getSize().y
                );
                // Сбрасываем позицию фона в 0,0 (на всякий случай)
                background.setPosition(0, 0);

                // 4. Двигаем кнопки в новый центр
                splashSprite.setPosition(newCenterX, newCenterY);
                centerText(titleText, newCenterX, event.size.height * 0.15f); // Заголовок всегда сверху (15%)
                centerText(btnRegister, newCenterX, newCenterY - 20);
                centerText(btnLogin, newCenterX, newCenterY + 50);
                centerText(btnExit, newCenterX, newCenterY + 120);
            }

            // КЛИКИ (Теперь работают идеально при любом размере)
            if (currentState == MAIN_MENU && event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    // mapPixelToCoords обязательно! Оно переводит пиксели экрана в координаты мира
                    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

                    if (btnRegister.getGlobalBounds().contains(mousePos)) {
                        std::cout << "CLICK: Registration" << std::endl;
                    }
                    else if (btnLogin.getGlobalBounds().contains(mousePos)) {
                        std::cout << "CLICK: Login" << std::endl;
                    }
                    else if (btnExit.getGlobalBounds().contains(mousePos)) {
                        window.close();
                    }
                }
            }
        }

        // --- ЛОГИКА ---
        if (currentState == SPLASH_SCREEN) {
            if (splashTimer.getElapsedTime().asSeconds() > 3.0f) {
                currentState = MAIN_MENU;
            }
        }

        // --- ОТРИСОВКА ---
        window.clear();

        if (currentState == SPLASH_SCREEN) {
            window.draw(splashSprite);
        }
        else if (currentState == MAIN_MENU) {
            window.draw(background);
            window.draw(titleText);
            window.draw(btnRegister);
            window.draw(btnLogin);
            window.draw(btnExit);
        }

        window.display();
    }

    return 0;
}