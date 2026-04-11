#include "MainMenuState.h"
#include <iostream>

using namespace std;

MainMenuState::MainMenuState() {
    // 1. Загрузка шрифта
    if (!mainFont.loadFromFile("assets/font.ttf")) {
        cout << "ERROR: Font not found in assets folder!" << endl;
    }

    float centerX = VideoMode::getDesktopMode().width / 2.0f;
    
    // 2. Настройка кнопок
    loginBtn.setFont(mainFont);
    loginBtn.setString(L"Вход");
    loginBtn.setCharacterSize(60);
    loginBtn.setFillColor(Color::White);
    loginBtn.setPosition(centerX, 400.0f);
    centerText(loginBtn);

    registerBtn.setFont(mainFont);
    registerBtn.setString(L"Регистрация");
    registerBtn.setCharacterSize(60);
    registerBtn.setFillColor(Color::White);
    registerBtn.setPosition(centerX, 550.0f);
    centerText(registerBtn);

    exitBtn.setFont(mainFont);
    exitBtn.setString(L"Выход");
    exitBtn.setCharacterSize(60);
    exitBtn.setFillColor(Color::White);
    exitBtn.setPosition(centerX, 700.0f);
    centerText(exitBtn);
}

void MainMenuState::centerText(Text& text) {
    FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
}

int MainMenuState::update(RenderWindow& window, Event& event) { // Изменили void на int
    static Clock animClock;
    float dt = animClock.restart().asSeconds();

    Vector2i mousePos = Mouse::getPosition(window);
    Vector2f mousePosF = window.mapPixelToCoords(mousePos);

    Text* buttons[] = { &loginBtn, &registerBtn, &exitBtn };
    int nextState = 0; // По умолчанию остаемся здесь

    for (auto* btn : buttons) {
        bool hovered = btn->getGlobalBounds().contains(mousePosF);
        float targetScale = hovered ? 1.2f : 1.0f;
        Color targetColor = hovered ? Color::Yellow : Color::White;

        // Плавная анимация
        float nextScale = btn->getScale().x + (targetScale - btn->getScale().x) * 8.0f * dt;
        btn->setScale(nextScale, nextScale);

        Color cur = btn->getFillColor();
        btn->setFillColor(Color(
            cur.r + (targetColor.r - cur.r) * 8.0f * dt,
            cur.g + (targetColor.g - cur.g) * 8.0f * dt,
            cur.b + (targetColor.b - cur.b) * 8.0f * dt
        ));

        // ОБРАБОТКА КЛИКА
        if (hovered && Mouse::isButtonPressed(Mouse::Left)) {
            if (btn == &registerBtn) return 1; // Переход в регистрацию
            if (btn == &exitBtn) window.close();
            // Здесь можно добавить return 2 для окна Входа
        }
    }
    return 0; // Продолжаем работу в меню
}

void MainMenuState::render(RenderWindow& window) {
    window.draw(loginBtn);
    window.draw(registerBtn);
    window.draw(exitBtn);
}