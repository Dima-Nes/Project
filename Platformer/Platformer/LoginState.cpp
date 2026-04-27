#include "LoginState.h"

// ─── Конструктор ─────────────────────────────────────────────────────────────

LoginState::LoginState(Database* database)
    : db(database), activeField(0), caretVisible(true)
{
    font.loadFromFile("assets/font.ttf");

    float W = (float)VideoMode::getDesktopMode().width;
    float H = (float)VideoMode::getDesktopMode().height;
    cx = W / 2.f;
    float left = cx - 250.f;
    float baseY = H * 0.22f;
    float gap = 140.f;

    // Заголовок
    title.setFont(font);
    title.setString(L"Вход в аккаунт");
    title.setCharacterSize(68);
    title.setFillColor(Color::White);
    title.setPosition(cx, baseY);
    centerText(title);

    // Поле «Логин»
    lblLogin.setFont(font);
    lblLogin.setString(L"Логин:");
    lblLogin.setCharacterSize(26);
    lblLogin.setFillColor(Color(190, 190, 210));
    lblLogin.setPosition(left, baseY + 80.f);

    boxLogin.setSize({ 500.f, 56.f });
    boxLogin.setPosition(left, baseY + 108.f);
    boxLogin.setFillColor(Color(45, 45, 58));
    boxLogin.setOutlineThickness(2.f);
    boxLogin.setOutlineColor(Color(90, 90, 120));

    fldLogin.setFont(font);
    fldLogin.setCharacterSize(30);
    fldLogin.setFillColor(Color::White);
    fldLogin.setPosition(left + 10.f, baseY + 118.f);

    // Поле «Пароль»
    lblPass.setFont(font);
    lblPass.setString(L"Пароль:");
    lblPass.setCharacterSize(26);
    lblPass.setFillColor(Color(190, 190, 210));
    lblPass.setPosition(left, baseY + 80.f + gap);

    boxPass.setSize({ 500.f, 56.f });
    boxPass.setPosition(left, baseY + 108.f + gap);
    boxPass.setFillColor(Color(45, 45, 58));
    boxPass.setOutlineThickness(2.f);
    boxPass.setOutlineColor(Color(90, 90, 120));

    fldPass.setFont(font);
    fldPass.setCharacterSize(30);
    fldPass.setFillColor(Color::White);
    fldPass.setPosition(left + 10.f, baseY + 118.f + gap);

    // Курсор
    caret.setSize({ 2.f, 34.f });
    caret.setFillColor(Color::White);

    // Кнопки
    btnSubmit.setFont(font);
    btnSubmit.setString(L"Войти");
    btnSubmit.setCharacterSize(52);
    btnSubmit.setFillColor(Color::White);
    btnSubmit.setPosition(cx, baseY + 108.f + gap * 2 + 25.f);
    centerText(btnSubmit);

    btnBack.setFont(font);
    btnBack.setString(L"Назад");
    btnBack.setCharacterSize(38);
    btnBack.setFillColor(Color(160, 160, 180));
    btnBack.setPosition(cx, baseY + 108.f + gap * 2 + 100.f);
    centerText(btnBack);

    // Ошибка
    msgError.setFont(font);
    msgError.setCharacterSize(26);
    msgError.setFillColor(Color(255, 80, 80));
    msgError.setPosition(left, baseY + 108.f + gap * 2 - 15.f);
}

// ─── Вспомогательные методы ───────────────────────────────────────────────────

void LoginState::centerText(Text& t) {
    FloatRect r = t.getLocalBounds();
    t.setOrigin(r.left + r.width / 2.f, r.top + r.height / 2.f);
}

void LoginState::refreshFields() {
    fldLogin.setString(sLogin);
    fldPass.setString(sPass);
}

// ─── update (события) ────────────────────────────────────────────────────────
// Возвращает:  0 = назад,  2 = успешный вход,  -1 = остаёмся здесь

int LoginState::update(RenderWindow& window, Event& event) {
    Vector2f mouse = window.mapPixelToCoords(Mouse::getPosition(window));

    // ── Клавиатура ──
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

    // ── Ввод символов ──
    if (event.type == Event::TextEntered) {
        sf::Uint32 c = event.text.unicode;
        if (c >= 32 && c < 128) {
            std::string& s = (activeField == 0) ? sLogin : sPass;
            if (s.size() < 32) s += static_cast<char>(c);
        }
    }

    // ── Мышь ──
    if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
        if (boxLogin.getGlobalBounds().contains(mouse)) { activeField = 0; msgError.setString(""); }
        else if (boxPass.getGlobalBounds().contains(mouse)) { activeField = 1; msgError.setString(""); }

        if (btnBack.getGlobalBounds().contains(mouse)) return 0;
        if (btnSubmit.getGlobalBounds().contains(mouse)) {
            if (db->loginUser(sLogin, sPass)) return 2;
            else msgError.setString(L"Неверный логин или пароль");
        }
    }

    refreshFields();
    return -1;
}

// ─── updateLogic (каждый кадр) ───────────────────────────────────────────────

void LoginState::updateLogic(RenderWindow& window) {
    // Мигание
    if (caretClock.getElapsedTime().asSeconds() > 0.5f) {
        caretVisible = !caretVisible;
        caretClock.restart();
    }

    // Позиция курсора
    Text* activeText = (activeField == 0) ? &fldLogin : &fldPass;
    std::string& activeStr = (activeField == 0) ? sLogin : sPass;

    Vector2f pos = activeText->getPosition();
    float    tw = activeStr.empty()
        ? 0.f
        : activeText->findCharacterPos(activeStr.size()).x - pos.x;
    caret.setPosition(pos.x + tw + 2.f, pos.y + 5.f);
}

// ─── render ──────────────────────────────────────────────────────────────────

void LoginState::render(RenderWindow& window) {
    boxLogin.setOutlineColor(activeField == 0 ? Color(100, 160, 255) : Color(90, 90, 120));
    boxPass.setOutlineColor(activeField == 1 ? Color(100, 160, 255) : Color(90, 90, 120));

    window.draw(title);

    window.draw(lblLogin);
    window.draw(boxLogin);
    window.draw(fldLogin);

    window.draw(lblPass);
    window.draw(boxPass);
    window.draw(fldPass);

    if (caretVisible) window.draw(caret);

    window.draw(btnSubmit);
    window.draw(btnBack);
    window.draw(msgError);
}