#include "RegistrationState.h"

RegistrationState::RegistrationState(Database* database, PlanetBackground* planet)
    : db(database), planet(planet)
{
    font.loadFromFile("assets/font.ttf");

    float W = (float)VideoMode::getDesktopMode().width;
    float H = (float)VideoMode::getDesktopMode().height;
    cx = W / 2.f;
    float left = cx - 250.f;
    float boxW = 500.f, boxH = 52.f;

    // ── Заголовок ─────────────────────────────────────────────────────────────
    title.setFont(font); title.setCharacterSize(64);
    title.setFillColor(Color::White);
    title.setString(L"Регистрация");
    title.setPosition(cx, H * 0.16f);
    centerText(title);

    // ── Логин ─────────────────────────────────────────────────────────────────
    float y0 = H * 0.28f;

    lblLogin.setFont(font); lblLogin.setCharacterSize(22);
    lblLogin.setFillColor(Color(190, 190, 210));
    lblLogin.setString(L"Логин:");
    lblLogin.setPosition(left, y0);

    boxLogin.setSize({ boxW,boxH });
    boxLogin.setPosition(left, y0 + 26.f);
    boxLogin.setFillColor(Color(45, 45, 58));
    boxLogin.setOutlineThickness(2.f);
    boxLogin.setOutlineColor(Color(90, 90, 120));

    fldLogin.setFont(font); fldLogin.setCharacterSize(26);
    fldLogin.setFillColor(Color::White);
    fldLogin.setPosition(left + 10.f, y0 + 36.f);

    // ── Пароль ────────────────────────────────────────────────────────────────
    float y1 = y0 + boxH + 52.f;

    lblPass.setFont(font); lblPass.setCharacterSize(22);
    lblPass.setFillColor(Color(190, 190, 210));
    lblPass.setString(L"Пароль (минимум 6 символов):");
    lblPass.setPosition(left, y1);

    boxPass.setSize({ boxW,boxH });
    boxPass.setPosition(left, y1 + 26.f);
    boxPass.setFillColor(Color(45, 45, 58));
    boxPass.setOutlineThickness(2.f);
    boxPass.setOutlineColor(Color(90, 90, 120));

    fldPass.setFont(font); fldPass.setCharacterSize(26);
    fldPass.setFillColor(Color::White);
    fldPass.setPosition(left + 10.f, y1 + 36.f);

    // ── Подтверждение ─────────────────────────────────────────────────────────
    float y2 = y1 + boxH + 52.f;

    lblConfirm.setFont(font); lblConfirm.setCharacterSize(22);
    lblConfirm.setFillColor(Color(190, 190, 210));
    lblConfirm.setString(L"Подтвердите пароль:");
    lblConfirm.setPosition(left, y2);

    boxConfirm.setSize({ boxW,boxH });
    boxConfirm.setPosition(left, y2 + 26.f);
    boxConfirm.setFillColor(Color(45, 45, 58));
    boxConfirm.setOutlineThickness(2.f);
    boxConfirm.setOutlineColor(Color(90, 90, 120));

    fldConfirm.setFont(font); fldConfirm.setCharacterSize(26);
    fldConfirm.setFillColor(Color::White);
    fldConfirm.setPosition(left + 10.f, y2 + 36.f);

    // ── Курсор ────────────────────────────────────────────────────────────────
    caret.setSize({ 2.f, 30.f });
    caret.setFillColor(Color::White);

    // ── Ошибка ────────────────────────────────────────────────────────────────
    float errorY = y2 + boxH + 16.f;
    msgError.setFont(font); msgError.setCharacterSize(22);
    msgError.setFillColor(Color(255, 80, 80));
    msgError.setPosition(left, errorY);

    // ── Кнопки ────────────────────────────────────────────────────────────────
    float submitY = errorY + 48.f;

    btnSubmit.setFont(font); btnSubmit.setCharacterSize(48);
    btnSubmit.setFillColor(Color::White);
    btnSubmit.setString(L"Создать аккаунт");
    btnSubmit.setPosition(cx, submitY);
    centerText(btnSubmit);

    btnBack.setFont(font); btnBack.setCharacterSize(34);
    btnBack.setFillColor(Color(160, 160, 180));
    btnBack.setString(L"Назад");
    btnBack.setPosition(cx, submitY + 74.f);
    centerText(btnBack);
}

void RegistrationState::centerText(Text& t) {
    FloatRect r = t.getLocalBounds();
    t.setOrigin(r.left + r.width / 2.f, r.top + r.height / 2.f);
}

void RegistrationState::refreshFields() {
    fldLogin.setString(sLogin);
    fldPass.setString(std::string(sPass.size(), '*'));
    fldConfirm.setString(std::string(sConfirm.size(), '*'));
}

void RegistrationState::setError(const wchar_t* msg) {
    msgError.setString(msg);
}

bool RegistrationState::trySubmit() {
    if (sLogin.empty()) { setError(L"Введите логин"); return false; }
    if ((int)sPass.size() < 6) { setError(L"Пароль должен быть минимум 6 символов"); return false; }
    if (sPass != sConfirm) { setError(L"Пароли не совпадают"); return false; }
    if (!db->registerUser(sLogin, sPass)) { setError(L"Этот логин уже занят"); return false; }
    return true;
}

int RegistrationState::update(RenderWindow& window, Event& event) {
    Vector2f mouse = window.mapPixelToCoords(Mouse::getPosition(window));

    if (event.type == Event::KeyPressed) {
        if (event.key.code == Keyboard::Escape) return 0;
        if (event.key.code == Keyboard::Tab)    activeField = (activeField + 1) % 3;
        if (event.key.code == Keyboard::Return) { if (trySubmit()) return 2; }
        if (event.key.code == Keyboard::BackSpace) {
            std::string* s = (activeField == 0) ? &sLogin : (activeField == 1) ? &sPass : &sConfirm;
            if (!s->empty()) s->pop_back();
        }
    }

    if (event.type == Event::TextEntered) {
        sf::Uint32 c = event.text.unicode;
        if (c >= 32 && c < 128) {
            std::string* s = (activeField == 0) ? &sLogin : (activeField == 1) ? &sPass : &sConfirm;
            size_t maxLen = (activeField == 0) ? 20 : 32;
            if (s->size() < maxLen) *s += static_cast<char>(c);
        }
    }

    if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
        if (boxLogin.getGlobalBounds().contains(mouse)) { activeField = 0; msgError.setString(""); }
        else if (boxPass.getGlobalBounds().contains(mouse)) { activeField = 1; msgError.setString(""); }
        else if (boxConfirm.getGlobalBounds().contains(mouse)) { activeField = 2; msgError.setString(""); }
        if (btnBack.getGlobalBounds().contains(mouse))   return 0;
        if (btnSubmit.getGlobalBounds().contains(mouse)) { if (trySubmit()) return 2; }
    }

    refreshFields();
    return -1;
}

void RegistrationState::updateLogic(RenderWindow& window) {
    // ── Мигание курсора ───────────────────────────────────────────────────────
    if (caretClock.getElapsedTime().asSeconds() > 0.5f) {
        caretVisible = !caretVisible;
        caretClock.restart();
    }
    Text& activeText = (activeField == 0) ? fldLogin : (activeField == 1) ? fldPass : fldConfirm;
    std::string& activeStr = (activeField == 0) ? sLogin : (activeField == 1) ? sPass : sConfirm;
    Vector2f pos = activeText.getPosition();
    float tw = activeStr.empty()
        ? 0.f
        : activeText.findCharacterPos(activeStr.size()).x - pos.x;
    caret.setPosition(pos.x + tw + 2.f, pos.y + 5.f);

    // ── Hover-анимация кнопок ─────────────────────────────────────────────────
    float dt = animClock.restart().asSeconds();
    Vector2f mouse = window.mapPixelToCoords(Mouse::getPosition(window));
    Text* buttons[] = { &btnSubmit, &btnBack };

    for (auto* btn : buttons) {
        bool  hov = btn->getGlobalBounds().contains(mouse);
        float ts = btn->getScale().x + ((hov ? 1.10f : 1.f) - btn->getScale().x) * 8.f * dt;
        btn->setScale(ts, ts);

        Color tgt = hov ? Color::Yellow : Color::White;
        Color cur = btn->getFillColor();
        btn->setFillColor(Color(
            (sf::Uint8)(cur.r + (tgt.r - cur.r) * 8.f * dt),
            (sf::Uint8)(cur.g + (tgt.g - cur.g) * 8.f * dt),
            (sf::Uint8)(cur.b + (tgt.b - cur.b) * 8.f * dt)
        ));
    }

    // ── Подсветка полей ───────────────────────────────────────────────────────
    RectangleShape* boxes[] = { &boxLogin, &boxPass, &boxConfirm };
    for (int i = 0; i < 3; ++i)
        boxes[i]->setOutlineColor(i == activeField ? Color(100, 160, 255) : Color(90, 90, 120));
}

void RegistrationState::render(RenderWindow& window) {
    if (planet) planet->render(window);

    window.draw(title);
    window.draw(lblLogin);   window.draw(boxLogin);   window.draw(fldLogin);
    window.draw(lblPass);    window.draw(boxPass);    window.draw(fldPass);
    window.draw(lblConfirm); window.draw(boxConfirm); window.draw(fldConfirm);
    if (caretVisible) window.draw(caret);
    window.draw(msgError);
    window.draw(btnSubmit);
    window.draw(btnBack);
}