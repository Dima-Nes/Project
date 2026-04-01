#pragma once
#include "State.h"

class MainMenuState : public State {
private:
    Font mainFont;
    Text loginBtn;
    Text registerBtn;
    Text exitBtn;

    // Вспомогательная функция, чтобы не дублировать код центрирования
    void centerText(Text& text);

public:
    MainMenuState(); // Конструктор (тут всё настроим)

    void update(RenderWindow& window) override;
    void render(RenderWindow& window) override;
};