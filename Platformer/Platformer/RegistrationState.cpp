#include "RegistrationState.h"

RegistrationState::RegistrationState() {
    mainFont.loadFromFile("assets/font.ttf");
    float centerX = VideoMode::getDesktopMode().width / 2.0f;

    // Заголовок
    title.setFont(mainFont);
    title.setString(L"Регистрация");
    title.setCharacterSize(80);
    title.setPosition(centerX, 150.0f);
    centerText(title);

    // Настройка трех полей ввода
    wstring labels[] = { L"Имя пользователя:", L"Пароль:", L"Повторите пароль:" };
    for (int i = 0; i < 3; i++) {
        fields[i].isActive = false;

        // Рамка поля
        fields[i].box.setSize(Vector2f(500.0f, 60.0f));
        fields[i].box.setFillColor(Color(50, 50, 50));
        fields[i].box.setOutlineThickness(2);
        fields[i].box.setOutlineColor(Color::White);
        fields[i].box.setOrigin(250.0f, 30.0f);
        fields[i].box.setPosition(centerX, 350.0f + i * 150.0f);

        // Текст-подсказка сверху
        fields[i].label.setFont(mainFont);
        fields[i].label.setString(labels[i]);
        fields[i].label.setCharacterSize(30);
        fields[i].label.setPosition(centerX - 250.0f, 290.0f + i * 150.0f);

        // Текст, который вводит пользователь
        fields[i].userInput.setFont(mainFont);
        fields[i].userInput.setCharacterSize(40);
        fields[i].userInput.setFillColor(Color::White);
        fields[i].userInput.setPosition(centerX - 240.0f, 325.0f + i * 150.0f);
    }

    // Кнопки
    backBtn.setFont(mainFont);
    backBtn.setString(L"Назад");
    backBtn.setCharacterSize(50);
    backBtn.setPosition(centerX - 200.0f, 850.0f);
    centerText(backBtn);

    exitBtn.setFont(mainFont);
    exitBtn.setString(L"Выход");
    exitBtn.setCharacterSize(50);
    exitBtn.setPosition(centerX + 200.0f, 850.0f);
    centerText(exitBtn);
}

void RegistrationState::centerText(Text& text) {
    FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
}

void RegistrationState::update(RenderWindow& window, Event& event) {
    static Clock animClock;
    float dt = animClock.restart().asSeconds();
    Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));

    // 1. Обработка ввода текста (через Event)
    //if (event.type == Event::TextEntered && activeFieldIndex != -1) {
    //    if (event.text.unicode == 8) { // Backspace
    //        if (!fields[activeFieldIndex].content.empty())
    //            fields[activeFieldIndex].content.pop_back();
    //    }
    //    else if (event.text.unicode < 128 || event.text.unicode > 159) { // Обычные символы
    //        fields[activeFieldIndex].content += static_cast<wchar_t>(event.text.unicode);
    //    }
    //    fields[activeFieldIndex].userInput.setString(fields[activeFieldIndex].content);
    //}
    // 1. Обработка ввода текста (через Event)
    if (event.type == Event::TextEntered && activeFieldIndex != -1) {
        if (event.text.unicode == 8) { // Backspace
            if (!fields[activeFieldIndex].content.empty()) {
                fields[activeFieldIndex].content.pop_back();
            }
        }
        // Проверка: код символа должен быть печатным и длина строки меньше 10
        else if (event.text.unicode >= 32 && fields[activeFieldIndex].content.size() < 14) {
            fields[activeFieldIndex].content += static_cast<wchar_t>(event.text.unicode);
        }

        fields[activeFieldIndex].userInput.setString(fields[activeFieldIndex].content);

        // СБРОС СОБЫТИЯ: Чтобы символ не вводился каждый кадр, 
        // превращаем обработанное событие в тип, который мы игнорируем.
        event.type = Event::Count;
    }

    // 2. Логика кнопок и полей (Анимация и клики)
    Text* uiButtons[] = { &backBtn, &exitBtn };
    for (auto* btn : uiButtons) {
        bool hovered = btn->getGlobalBounds().contains(mousePos);
        float targetScale = hovered ? 1.1f : 1.0f;
        Color targetColor = hovered ? Color::Yellow : Color::White;

        // Плавный масштаб и цвет
        float nextScale = btn->getScale().x + (targetScale - btn->getScale().x) * 10.0f * dt;
        btn->setScale(nextScale, nextScale);

        Color cur = btn->getFillColor();
        btn->setFillColor(Color(
            cur.r + (targetColor.r - cur.r) * 10.0f * dt,
            cur.g + (targetColor.g - cur.g) * 10.0f * dt,
            cur.b + (targetColor.b - cur.b) * 10.0f * dt
        ));

        if (hovered && Mouse::isButtonPressed(Mouse::Left)) {
            if (btn == &exitBtn) window.close();
            // Здесь будет логика возврата в меню
        }
    }

    // Клик по полю ввода для активации
    if (Mouse::isButtonPressed(Mouse::Left)) {
        activeFieldIndex = -1;
        for (int i = 0; i < 3; i++) {
            if (fields[i].box.getGlobalBounds().contains(mousePos)) {
                activeFieldIndex = i;
                fields[i].box.setOutlineColor(Color::Yellow);
            }
            else {
                fields[i].box.setOutlineColor(Color::White);
            }
        }
    }
}

void RegistrationState::render(RenderWindow& window) {
    window.draw(title);
    for (int i = 0; i < 3; i++) {
        window.draw(fields[i].box);
        window.draw(fields[i].label);
        window.draw(fields[i].userInput);
    }
    window.draw(backBtn);
    window.draw(exitBtn);
}