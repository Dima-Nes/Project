#include "RegistrationState.h"

RegistrationState::RegistrationState() {
    mainFont.loadFromFile("assets/font.ttf");
    float centerX = VideoMode::getDesktopMode().width / 2.0f;

    title.setFont(mainFont);
    title.setString(L"Регистрация");
    title.setCharacterSize(80);
    title.setPosition(centerX, 150.0f);
    centerText(title);

    wstring labels[] = { L"Имя пользователя:", L"Пароль:", L"Повторите пароль:" };
    for (int i = 0; i < 3; i++) {
        fields[i].box.setSize(Vector2f(500.0f, 60.0f));
        fields[i].box.setFillColor(Color(50, 50, 50));
        fields[i].box.setOutlineThickness(2);
        fields[i].box.setOutlineColor(Color::White);
        fields[i].box.setOrigin(250.0f, 30.0f);
        fields[i].box.setPosition(centerX, 350.0f + i * 150.0f);

        fields[i].label.setFont(mainFont);
        fields[i].label.setString(labels[i]);
        fields[i].label.setCharacterSize(30);
        fields[i].label.setPosition(centerX - 250.0f, 290.0f + i * 150.0f);

        fields[i].userInput.setFont(mainFont);
        fields[i].userInput.setCharacterSize(40);
        fields[i].userInput.setFillColor(Color::White);
        // Смещаем текст чуть правее края рамки
        fields[i].userInput.setPosition(centerX - 240.0f, 325.0f + i * 150.0f);
        fields[i].content = L"";
    }

    // Настройка курсора
    cursor.setSize(Vector2f(2.0f, 40.0f));
    cursor.setFillColor(Color::Yellow);

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

int RegistrationState::update(RenderWindow& window, Event& event) {
    Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));

    // --- ЛОГИКА АНИМАЦИИ (выполняется всегда) ---
    // Подсветка кнопок при наведении
    if (backBtn.getGlobalBounds().contains(mousePos)) backBtn.setFillColor(Color::Yellow);
    else backBtn.setFillColor(Color::White);

    if (exitBtn.getGlobalBounds().contains(mousePos)) exitBtn.setFillColor(Color::Red);
    else exitBtn.setFillColor(Color::White);

    // Мигание курсора
    if (cursorClock.getElapsedTime().asSeconds() >= 0.5f) {
        showCursor = !showCursor;
        cursorClock.restart();
    }

    // --- ЛОГИКА СОБЫТИЙ (только когда что-то произошло) ---
    if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
        // Клик по кнопке "Назад"
        if (backBtn.getGlobalBounds().contains(mousePos)) return 0;

        // Клик по кнопке "Выход"
        if (exitBtn.getGlobalBounds().contains(mousePos)) window.close();

        // Выбор поля ввода
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

    // Ввод текста (теперь сработает 1 раз за нажатие)
    if (event.type == Event::TextEntered && activeFieldIndex != -1) {
        if (event.text.unicode == 8) { // Backspace
            if (!fields[activeFieldIndex].content.empty())
                fields[activeFieldIndex].content.pop_back();
        }
        else if (event.text.unicode < 128 && fields[activeFieldIndex].content.size() < maxChars) {
            fields[activeFieldIndex].content += static_cast<wchar_t>(event.text.unicode);
        }
        fields[activeFieldIndex].userInput.setString(fields[activeFieldIndex].content);

        // Обновляем позицию курсора
        float x = fields[activeFieldIndex].userInput.getGlobalBounds().left +
            fields[activeFieldIndex].userInput.getGlobalBounds().width + 5;
        float y = fields[activeFieldIndex].userInput.getPosition().y + 5;
        cursor.setPosition(x, y);
    }

    return -1; // Остаемся в этом окне
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

    if (activeFieldIndex != -1 && showCursor) {
        window.draw(cursor);
    }
}