#pragma once
#include "State.h"

class GameMenuState : public State {
private:
    Font font;
    float cx;

    Text title;
    Text btnPlay, btnSettings, btnNewWorld, btnExit;

    Clock animClock;

    void centerText(Text& t);

public:
    GameMenuState();

    int update(RenderWindow& window, Event& event) override;
    void updateLogic(RenderWindow& window); // Анимации кнопок
    void render(RenderWindow& window) override;
};