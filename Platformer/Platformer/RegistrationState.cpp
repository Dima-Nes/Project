#include "RegistrationState.h"

// ─── Вспомогательные функции ─────────────────────────────────────────────────

static void makeLabel(Text& t, Font& f, const wchar_t* s, float x, float y) {
    t.setFont(f);
    t.setString(s);
    t.setCharacterSize(26);
    t.setFillColor(Color(190, 190, 210));
    t.setPosition(x, y);
}

static void makeBox(RectangleShape& b, float x, float y) {
    b.setSize({ 500.f, 56.f });
    b.setPosition(x, y);
    b.setFillColor(Color(45, 45, 58));
    b.setOutlineThickness(2.f);
    b.setOutlineColor(Color(90, 90, 120));
}

static void makeFieldText(Text& t, Font& f, float x, float y) {
    t.setFont(f);
    t.setCharacterSize(30);
    t.setFillColor(Color::White);
    t.setPosition(x + 10.f, y + 10.f);
}

// ─── Конструктор ─────────────────────────────────────────────────────────────

RegistrationState::RegistrationState(Database* database)
    : db(database), activeField(0), caretVisible(true)
{
    font.loadFromFile("assets/font.ttf");

    float W = (float)VideoMode::getDesktopMode().width;
    float H = (float)VideoMode::getDesktopMode().height;
    cx = W / 2.f;
    float left = cx - 250.f;

    float baseY = H * 0.12f;
    float gap = 125.f;

    // Заголовок
    title.setFont(font);
    title.setString(L"Регистрация");
    title.setCharacterSize(68);
    title.setFillColor(Color::White);
    title.setPosition(cx, baseY);
    centerText(title);

    // Поля
    makeLabel(lblLogin, font, L"Логин:", left, baseY + 70.f);
    makeBox(boxLogin, left, baseY + 103.f);
    makeFieldText(fldLogin, font, left, baseY + 103.f);

    makeLabel(lblPass, font, L"Пароль (минимум 6 символов):", left, baseY + 70.f + gap);
    makeBox(boxPass, left, baseY + 103.f + gap);
    makeFieldText(fldPass, font, left, baseY + 103.f + gap);

    makeLabel(lblConfirm, font, L"Подтвердите пароль:", left, baseY + 70.f + gap * 2);
    makeBox(boxConfirm, left, baseY + 103.f + gap * 2);
    makeFieldText(fldConfirm, font, left, baseY + 103.f + gap * 2);

    // Курсор
    caret.setSize({ 2.f, 34.f });
    caret.setFillColor(Color::White);

    // Кнопки
    btnSubmit.setFont(font);
    btnSubmit.setString(L"Создать аккаунт");
    btnSubmit.setCharacterSize(50);
    btnSubmit.setFillColor(Color::White);
    btnSubmit.setPosition(cx, baseY + 103.f + gap * 3 + 10.f);
    centerText(btnSubmit);

    btnBack.setFont(font);
    btnBack.setString(L"Назад");
    btnBack.setCharacterSize(38);
    btnBack.setFillColor(Color(160, 160, 180));
    btnBack.setPosition(cx, baseY + 103.f + gap * 3 + 82.f);
    centerText(btnBack);

    // Ошибка
    msgError.setFont(font);
    msgError.setCharacterSize(26);
    msgError.setFillColor(Color(255, 80, 80));
    msgError.setPosition(left, baseY + 103.f + gap * 3 - 32.f);
}

// ─── Вспомогательные методы ───────────────────────────────────────────────────

void RegistrationState::centerText(Text& t) {
    FloatRect r = t.getLocalBounds();
    t.setOrigin(r.left + r.width / 2.f, r.top + r.height / 2.f);
}

void RegistrationState::refreshFields() {
    fldLogin.setString(sLogin);
    fldPass.setString(sPass);
    fldConfirm.setString(sConfirm);
}

void RegistrationState::setError(const wchar_t* msg) {
    msgError.setString(msg);
}

bool RegistrationState::trySubmit() {
    if (sLogin.empty()) {
        setError(L"Введите логин");
        return false;
    }
    if ((int)sPass.size() < 6) {
        setError(L"Пароль должен быть минимум 6 символов");
        return false;
    }
    if (sPass != sConfirm) {
        setError(L"Пароли не совпадают");
        return false;
    }
    if (!db->registerUser(sLogin, sPass)) {
        setError(L"Этот логин уже занят");
        return false;
    }
    return true;
}

// ─── update (обработка событий) ──────────────────────────────────────────────
// Возвращает:  0 = назад в меню,  2 = успешная регистрация,  -1 = остаёмся здесь

int RegistrationState::update(RenderWindow& window, Event& event) {
    Vector2f mouse = window.mapPixelToCoords(Mouse::getPosition(window));

    // ── Клавиатура ──
    if (event.type == Event::KeyPressed) {
        if (event.key.code == Keyboard::Escape)  return 0;
        if (event.key.code == Keyboard::Tab)     activeField = (activeField + 1) % 3;
        if (event.key.code == Keyboard::Return) { if (trySubmit()) return 2; }

        if (event.key.code == Keyboard::BackSpace) {
            std::string* s = (activeField == 0) ? &sLogin
                : (activeField == 1) ? &sPass
                : &sConfirm;
            if (!s->empty()) s->pop_back();
        }
    }

    // ── Ввод символов (только ASCII, чтобы не ломать логины) ──
    if (event.type == Event::TextEntered) {
        sf::Uint32 c = event.text.unicode;
        if (c >= 32 && c < 128) {
            std::string* s = (activeField == 0) ? &sLogin
                : (activeField == 1) ? &sPass
                : &sConfirm;
            size_t maxLen = (activeField == 0) ? 20 : 32;
            if (s->size() < maxLen)
                *s += static_cast<char>(c);
        }
    }

    // ── Мышь ──
    if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
        if (boxLogin.getGlobalBounds().contains(mouse)) { activeField = 0; msgError.setString(""); }
        else if (boxPass.getGlobalBounds().contains(mouse)) { activeField = 1; msgError.setString(""); }
        else if (boxConfirm.getGlobalBounds().contains(mouse)) { activeField = 2; msgError.setString(""); }

        if (btnBack.getGlobalBounds().contains(mouse))   return 0;
        if (btnSubmit.getGlobalBounds().contains(mouse)) { if (trySubmit()) return 2; }
    }

    refreshFields();
    return -1; // Остаёмся на этом экране
}

// ─── updateLogic (курсор — вызывается каждый кадр) ───────────────────────────

void RegistrationState::updateLogic(RenderWindow& window) {
    // Мигание курсора
    if (caretClock.getElapsedTime().asSeconds() > 0.5f) {
        caretVisible = !caretVisible;
        caretClock.restart();
    }

    // Позиция курсора — после последнего символа активного поля
    Text* activeText = (activeField == 0) ? &fldLogin
        : (activeField == 1) ? &fldPass
        : &fldConfirm;
    std::string& activeStr = (activeField == 0) ? sLogin
        : (activeField == 1) ? sPass
        : sConfirm;

    Vector2f pos = activeText->getPosition();
    float    tw = activeStr.empty()
        ? 0.f
        : activeText->findCharacterPos(activeStr.size()).x - pos.x;
    caret.setPosition(pos.x + tw + 2.f, pos.y + 5.f);
}

// ─── render ──────────────────────────────────────────────────────────────────

void RegistrationState::render(RenderWindow& window) {
    // Подсвечиваем активное поле синей рамкой
    RectangleShape* boxes[] = { &boxLogin, &boxPass, &boxConfirm };
    for (int i = 0; i < 3; i++)
        boxes[i]->setOutlineColor(i == activeField ? Color(100, 160, 255)
            : Color(90, 90, 120));

    window.draw(title);

    window.draw(lblLogin);
    window.draw(boxLogin);
    window.draw(fldLogin);

    window.draw(lblPass);
    window.draw(boxPass);
    window.draw(fldPass);

    window.draw(lblConfirm);
    window.draw(boxConfirm);
    window.draw(fldConfirm);

    if (caretVisible) window.draw(caret);

    window.draw(btnSubmit);
    window.draw(btnBack);
    window.draw(msgError);
}