#include "MainMenuState.h"
#include <iostream>

MainMenuState::MainMenuState() {
    // 1. ИСПРАВЛЕННЫЙ ПУТЬ: указываем папку assets и файл, который у тебя реально есть
    if (!mainFont.loadFromFile("assets/font.ttf")) {
        // Если font.ttf не сработает, попробуй "assets/Comic.ttf"
        cout << "ERROR: Font not found in assets folder!" << endl;
    }

    float centerX = VideoMode::getDesktopMode().width / 2.0f;

    // 2. Настройка кнопок
    loginBtn.setFont(mainFont);
    loginBtn.setString(L"Вход");
    loginBtn.setCharacterSize(60);
    loginBtn.setFillColor(Color::White); // Явно задаем цвет
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
    // Устанавливаем точку привязки в центр текста для корректной анимации увеличения
    text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
}

void MainMenuState::update(RenderWindow& window) {
    // ВАЖНО: Получаем позицию мыши ОТНОСИТЕЛЬНО окна, а не экрана
    Vector2i mousePos = Mouse::getPosition(window);
    Vector2f mousePosF = window.mapPixelToCoords(mousePos); // Это делает код адаптивным!

    Text* buttons[] = { &loginBtn, &registerBtn, &exitBtn };

    for (auto* btn : buttons) {
        if (btn->getGlobalBounds().contains(mousePosF)) {
            // Анимация наведения
            btn->setScale(1.2f, 1.2f);
            btn->setFillColor(Color::Yellow);

            if (Mouse::isButtonPressed(Mouse::Left)) {
                // Логика нажатий
            }
        }
        else {
            // Возврат в обычное состояние
            btn->setScale(1.0f, 1.0f);
            btn->setFillColor(Color::White);
        }
    }
}

void MainMenuState::render(RenderWindow& window) {
    window.draw(loginBtn);
    window.draw(registerBtn);
    window.draw(exitBtn);
}