#include "LoginState.h"

LoginState::LoginState(Database* database, PlanetBackground* planet)
    : db(database), planet(planet)
{
    font.loadFromFile("assets/font.ttf");

    float W = (float)VideoMode::getDesktopMode().width;
    float H = (float)VideoMode::getDesktopMode().height;
    cx = W / 2.f;

    float left = cx - 250.f;
    float boxW = 500.f;
    float boxH = 52.f;

    // ── Заголовок ─────────────────────────────────────────────────────────────
    title.setFont(font);
    title.setString(L"Вход в аккаунт");
    title.setCharacterSize(64);
    title.setFillColor(Color::White);
    title.setPosition(cx, H * 0.22f);
    centerText(title);

    // ── Поле «Логин» ──────────────────────────────────────────────────────────
    float loginLabelY = H * 0.38f;
    float loginBoxY = loginLabelY + 28.f;

    lblLogin.setFont(font); lblLogin.setCharacterSize(24);
    lblLogin.setFillColor(Color(190, 190, 210));
    lblLogin.setString(L"Логин:");
    lblLogin.setPosition(left, loginLabelY);

    boxLogin.setSize({ boxW, boxH });
    boxLogin.setPosition(left, loginBoxY);
    boxLogin.setFillColor(Color(45, 45, 58));
    boxLogin.setOutlineThickness(2.f);
    boxLogin.setOutlineColor(Color(90, 90, 120));

    fldLogin.setFont(font); fldLogin.setCharacterSize(28);
    fldLogin.setFillColor(Color::White);
    fldLogin.setPosition(left + 10.f, loginBoxY + 10.f);

    // ── Поле «Пароль» ─────────────────────────────────────────────────────────
    float passLabelY = loginBoxY + boxH + 22.f;
    float passBoxY = passLabelY + 28.f;

    lblPass.setFont(font); lblPass.setCharacterSize(24);
    lblPass.setFillColor(Color(190, 190, 210));
    lblPass.setString(L"Пароль:");
    lblPass.setPosition(left, passLabelY);

    boxPass.setSize({ boxW, boxH });
    boxPass.setPosition(left, passBoxY);
    boxPass.setFillColor(Color(45, 45, 58));
    boxPass.setOutlineThickness(2.f);
    boxPass.setOutlineColor(Color(90, 90, 120));

    fldPass.setFont(font); fldPass.setCharacterSize(28);
    fldPass.setFillColor(Color::White);
    fldPass.setPosition(left + 10.f, passBoxY + 10.f);

    // ── Курсор ────────────────────────────────────────────────────────────────
    caret.setSize({ 2.f, 30.f });
    caret.setFillColor(Color::White);

    // ── Ошибка ────────────────────────────────────────────────────────────────
    float errorY = passBoxY + boxH + 14.f;
    msgError.setFont(font); msgError.setCharacterSize(24);
    msgError.setFillColor(Color(255, 80, 80));
    msgError.setPosition(left, errorY);

    // ── Кнопки ────────────────────────────────────────────────────────────────
    float submitY = errorY + 50.f;

    btnSubmit.setFont(font); btnSubmit.setCharacterSize(50);
    btnSubmit.setFillColor(Color::White);
    btnSubmit.setString(L"Войти");
    btnSubmit.setPosition(cx, submitY);
    centerText(btnSubmit);

    btnBack.setFont(font); btnBack.setCharacterSize(36);
    btnBack.setFillColor(Color(160, 160, 180));
    btnBack.setString(L"Назад");
    btnBack.setPosition(cx, submitY + 80.f);
    centerText(btnBack);
}

void LoginState::centerText(Text& t) {
    FloatRect r = t.getLocalBounds();
    t.setOrigin(r.left + r.width / 2.f, r.top + r.height / 2.f);
}

void LoginState::refreshFields() {
    fldLogin.setString(sLogin);
    // Показываем пароль звёздочками
    fldPass.setString(std::string(sPass.size(), '*'));
}

int LoginState::update(RenderWindow& window, Event& event) {
    Vector2f mouse = window.mapPixelToCoords(Mouse::getPosition(window));

    if (event.type == Event::KeyPressed) {
        if (event.key.code == Keyboard::Escape) return 0;
        if (event.key.code == Keyboard::Tab)    activeField = (activeField + 1) % 2;
        if (event.key.code == Keyboard::BackSpace) {
            std::string& s = (activeField == 0) ? sLogin : sPass;
            if (!s.empty()) s.pop_back();
        }
        if (event.key.code == Keyboard::Return) {
            if (db->loginUser(sLogin, sPass)) return 2;
            else msgError.setString(L"Неверный логин или пароль");
        }
    }

    if (event.type == Event::TextEntered) {
        sf::Uint32 c = event.text.unicode;
        if (c >= 32 && c < 128) {
            std::string& s = (activeField == 0) ? sLogin : sPass;
            if (s.size() < 32) s += static_cast<char>(c);
        }
    }

    if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
        if (boxLogin.getGlobalBounds().contains(mouse)) { activeField = 0; msgError.setString(""); }
        else if (boxPass.getGlobalBounds().contains(mouse)) { activeField = 1; msgError.setString(""); }
        if (btnBack.getGlobalBounds().contains(mouse))   return 0;
        if (btnSubmit.getGlobalBounds().contains(mouse)) {
            if (db->loginUser(sLogin, sPass)) return 2;
            else msgError.setString(L"Неверный логин или пароль");
        }
    }

    refreshFields();
    return -1;
}

void LoginState::updateLogic(RenderWindow& window) {
    // ── Мигание курсора ───────────────────────────────────────────────────────
    if (caretClock.getElapsedTime().asSeconds() > 0.5f) {
        caretVisible = !caretVisible;
        caretClock.restart();
    }
    Text& activeText = (activeField == 0) ? fldLogin : fldPass;
    std::string& activeStr = (activeField == 0) ? sLogin : sPass;
    Vector2f pos = activeText.getPosition();
    float tw = activeStr.empty()
        ? 0.f
        : activeText.findCharacterPos(activeStr.size()).x - pos.x;
    caret.setPosition(pos.x + tw + 2.f, pos.y + 5.f);

    // ── Анимация кнопок (hover) ───────────────────────────────────────────────
    float dt = animClock.restart().asSeconds();
    Vector2f mouse = window.mapPixelToCoords(Mouse::getPosition(window));
    Text* buttons[] = { &btnSubmit, &btnBack };

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

    // ── Подсветка активного поля ──────────────────────────────────────────────
    boxLogin.setOutlineColor(activeField == 0 ? Color(100, 160, 255) : Color(90, 90, 120));
    boxPass.setOutlineColor(activeField == 1 ? Color(100, 160, 255) : Color(90, 90, 120));
}

void LoginState::render(RenderWindow& window) {
    if (planet) planet->render(window);

    window.draw(title);
    window.draw(lblLogin); window.draw(boxLogin); window.draw(fldLogin);
    window.draw(lblPass);  window.draw(boxPass);  window.draw(fldPass);
    if (caretVisible) window.draw(caret);
    window.draw(msgError);
    window.draw(btnSubmit);
    window.draw(btnBack);
}