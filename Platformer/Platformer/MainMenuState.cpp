#include "MainMenuState.h"

MainMenuState::MainMenuState(PlanetBackground* planet) : planet(planet) {
    if (!mainFont.loadFromFile("assets/font.ttf"))
        return;

    float W = (float)VideoMode::getDesktopMode().width;
    float H = (float)VideoMode::getDesktopMode().height;
    float cx = W / 2.f;

    // Кнопки опущены ниже — начинаются с 55% высоты экрана
    float startY = H * 0.42f;
    float gap = 110.f;

    auto setup = [&](Text& t, const wchar_t* s, float y) {
        t.setFont(mainFont);
        t.setString(s);
        t.setCharacterSize(58);
        t.setFillColor(Color::White);
        t.setPosition(cx, y);
        centerText(t);
        };

    setup(loginBtn, L"Вход", startY);
    setup(registerBtn, L"Регистрация", startY + gap);
    setup(exitBtn, L"Выход", startY + gap * 2);
}

void MainMenuState::centerText(Text& t) {
    FloatRect r = t.getLocalBounds();
    t.setOrigin(r.left + r.width / 2.f, r.top + r.height / 2.f);
}

void MainMenuState::updateLogic(RenderWindow& window) {
    float dt = animClock.restart().asSeconds();
    Vector2f mouse = window.mapPixelToCoords(Mouse::getPosition(window));
    Text* buttons[] = { &loginBtn, &registerBtn, &exitBtn };

    for (auto* btn : buttons) {
        bool  hov = btn->getGlobalBounds().contains(mouse);
        float ts = btn->getScale().x + ((hov ? 1.15f : 1.f) - btn->getScale().x) * 8.f * dt;
        btn->setScale(ts, ts);

        Color tgt = hov ? Color::Yellow : Color::White;
        Color cur = btn->getFillColor();
        btn->setFillColor(Color(
            (sf::Uint8)(cur.r + (tgt.r - cur.r) * 8.f * dt),
            (sf::Uint8)(cur.g + (tgt.g - cur.g) * 8.f * dt),
            (sf::Uint8)(cur.b + (tgt.b - cur.b) * 8.f * dt)
        ));
    }
}

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
    // 1. Планета
    if (planet) planet->render(window);

    // 2. Кнопки поверх
    window.draw(loginBtn);
    window.draw(registerBtn);
    window.draw(exitBtn);
}