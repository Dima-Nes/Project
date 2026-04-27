#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

using namespace sf;
using namespace std;

class LoginState {
private:
    Font mainFont;

    // Тексты для кнопок и заголовков
    Text title;
    Text backBtn, exitBtn;

    // Поля ввода: текст-подсказка и то, что ввел пользователь
    struct InputField {
        RectangleShape box;
        Text label;        // "Имя пользователя", "Пароль" и т.д.
        Text userInput;    // То, что печатает юзер
        wstring content;   // Сама строка ввода
        bool isActive;     // Выбрано ли поле сейчас
    };

    InputField fields[3]; // 0: Ник, 1: Пароль, 2: Подтверждение
    int activeFieldIndex = -1;

    void centerText(Text& text);

public:
    LoginState();
    int update(RenderWindow& window, Event& event);
    void render(RenderWindow& window);
};