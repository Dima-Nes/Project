#include "GameMenuState.h"

GameMenuState::GameMenuState(PlanetBackground* planet) : planet(planet) {
    font.loadFromFile("assets/font.ttf");

    float W = (float)VideoMode::getDesktopMode().width;
    float H = (float)VideoMode::getDesktopMode().height;
    cx = W / 2.f;

    title.setFont(font);
    title.setString(L"Главное меню");
    title.setCharacterSize(72);
    title.setFillColor(Color::White);
    title.setPosition(cx, H * 0.20f);
    centerText(title);

    // Кнопки начинаются с 48% высоты
    float startY = H * 0.48f;
    float gap = 110.f;

    auto setup = [&](Text& t, const wchar_t* s, float y) {
        t.setFont(font);
        t.setString(s);
        t.setCharacterSize(58);
        t.setFillColor(Color::White);
        t.setPosition(cx, y);
        centerText(t);
        };

    setup(btnPlay, L"Играть", startY);
    setup(btnSettings, L"Настройки", startY + gap);
    setup(btnNewWorld, L"Новый мир", startY + gap * 2);
    setup(btnExit, L"Выйти", startY + gap * 3);
}

void GameMenuState::centerText(Text& t) {
    FloatRect r = t.getLocalBounds();
    t.setOrigin(r.left + r.width / 2.f, r.top + r.height / 2.f);
}

void GameMenuState::updateLogic(RenderWindow& window) {
    float dt = animClock.restart().asSeconds();
    Vector2f mouse = window.mapPixelToCoords(Mouse::getPosition(window));
    Text* buttons[] = { &btnPlay, &btnSettings, &btnNewWorld, &btnExit };

    for (auto* btn : buttons) {
        bool  hov = btn->getGlobalBounds().contains(mouse);
        float ts = btn->getScale().x + ((hov ? 1.12f : 1.f) - btn->getScale().x) * 8.f * dt;
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

int GameMenuState::update(RenderWindow& window, Event& event) {
    if (event.type != Event::MouseButtonPressed) return -1;
    if (event.mouseButton.button != Mouse::Left)  return -1;

    Vector2f mouse = window.mapPixelToCoords(
        { event.mouseButton.x, event.mouseButton.y }
    );

    if (btnPlay.getGlobalBounds().contains(mouse))     return 1;
    if (btnSettings.getGlobalBounds().contains(mouse)) return 2;
    if (btnNewWorld.getGlobalBounds().contains(mouse)) return 3;
    if (btnExit.getGlobalBounds().contains(mouse))     return 0;
    return -1;
}

void GameMenuState::render(RenderWindow& window) {
    if (planet) planet->render(window);

    window.draw(title);
    window.draw(btnPlay);
    window.draw(btnSettings);
    window.draw(btnNewWorld);
    window.draw(btnExit);
}