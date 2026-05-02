#pragma once
#include "State.h"

class MainMenuState : public State {
private:
    Font mainFont;
    Text loginBtn;
    Text registerBtn;
    Text exitBtn;

    Clock animClock; // ← перенесён сюда из update()

    void centerText(Text& text);

public:
    MainMenuState();

    int  update(RenderWindow& window, Event& event) override;
    void updateLogic(RenderWindow& window); // ← НОВОЕ: анимации каждый кадр
    void render(RenderWindow& window) override;
};