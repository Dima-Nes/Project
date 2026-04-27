#pragma once
#include "State.h"

class MainMenuState : public State {
private:
    Font mainFont;
    Text loginBtn;
    Text registerBtn;
    Text exitBtn;

    void centerText(Text& text);

public:
    MainMenuState();

    int  update(RenderWindow& window, Event& event) override;
    void render(RenderWindow& window) override;

    // ─── НОВОЕ ────────────────────────────────────────────────────────────────
    // Hover-анимация кнопок — вызывать каждый кадр из Game::run()
    void updateLogic(RenderWindow& window);
};