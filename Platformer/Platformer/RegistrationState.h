//#pragma once
//#include <SFML/Graphics.hpp>
//#include <vector>
//#include <string>
//#include "State.h" // Обязательно подключаем базу
//
//using namespace sf;
//using namespace std;
//
//class RegistrationState : public State { // Добавляем наследование!
//private:
//    // ... твои переменные ...
//    void centerText(Text& text);
//
//public:
//    RegistrationState();
//
//    // Эту функцию вызываем ОДИН раз за кадр для анимаций
//    void updateLogic(RenderWindow& window);
//
//    // Эту функцию вызываем ВНУТРИ цикла pollEvent для ввода
//    int update(RenderWindow& window, Event& event) override;
//
//    void render(RenderWindow& window) override;
//};

#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "State.h" 

using namespace sf;
using namespace std;

// Структура для полей ввода (логин, пароль и т.д.)
struct InputField {
    RectangleShape box;
    Text label;
    Text userInput;
    wstring content;
};

class RegistrationState : public State {
private:
    // Ресурсы
    Font mainFont;
    Text title;

    // Поля ввода
    InputField fields[3];
    int activeFieldIndex = -1;

    // Кнопки
    RectangleShape backBtn;
    Text backText;
    RectangleShape exitBtn;
    Text exitText;

    // Курсор для текста
    RectangleShape cursor;
    Clock cursorClock;
    bool showCursor = true;
    const int maxChars = 20;

    void centerText(Text& text);

public:
    RegistrationState();

    // Логика анимаций (мигание курсора)
    void updateLogic(RenderWindow& window);

    // Обработка ввода и кликов
    int update(RenderWindow& window, Event& event) override;

    void render(RenderWindow& window) override;
};