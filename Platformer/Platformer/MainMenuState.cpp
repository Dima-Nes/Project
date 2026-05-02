#include "MainMenuState.h"
#include <iostream>

using namespace std;

MainMenuState::MainMenuState() {
    if (!mainFont.loadFromFile("assets/font.ttf"))
        cout << "ERROR: Font not found in assets folder!" << endl;

    float centerX = VideoMode::getDesktopMode().width / 2.0f;

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
    text.setOrigin(textRect.left + textRect.width / 2.0f,
        textRect.top + textRect.height / 2.0f);
}

// ─── updateLogic — вызывается каждый кадр из Game.cpp ────────────────────────

void MainMenuState::updateLogic(RenderWindow& window) {
    float dt = animClock.restart().asSeconds();

    Vector2f mouse = window.mapPixelToCoords(Mouse::getPosition(window));
    Text* buttons[] = { &loginBtn, &registerBtn, &exitBtn };

    for (auto* btn : buttons) {
        bool  hovered = btn->getGlobalBounds().contains(mouse);
        float targetScale = hovered ? 1.2f : 1.0f;
        Color targetColor = hovered ? Color::Yellow : Color::White;

        float s = btn->getScale().x + (targetScale - btn->getScale().x) * 8.0f * dt;
        btn->setScale(s, s);

        Color c = btn->getFillColor();
        btn->setFillColor(Color(
            (sf::Uint8)(c.r + (targetColor.r - c.r) * 8.0f * dt),
            (sf::Uint8)(c.g + (targetColor.g - c.g) * 8.0f * dt),
            (sf::Uint8)(c.b + (targetColor.b - c.b) * 8.0f * dt)
        ));
    }
}

// ─── update — только события/клики ───────────────────────────────────────────

int MainMenuState::update(RenderWindow& window, Event& event) {
    if (event.type != Event::MouseButtonPressed) return 0;
    if (event.mouseButton.button != Mouse::Left)  return 0;

    Vector2f mouse = window.mapPixelToCoords(Mouse::getPosition(window));

    if (loginBtn.getGlobalBounds().contains(mouse))    return 1;
    if (registerBtn.getGlobalBounds().contains(mouse)) return 2;
    if (exitBtn.getGlobalBounds().contains(mouse)) { window.close(); }

    return 0;
}

void MainMenuState::render(RenderWindow& window) {
    window.draw(loginBtn);
    window.draw(registerBtn);
    window.draw(exitBtn);
}