#pragma once
#include "State.h"
#include "PlanetBackground.h"

class MainMenuState : public State {
private:
    PlanetBackground* planet;   // не владеем — только рисуем

    Font mainFont;
    Text loginBtn;
    Text registerBtn;
    Text exitBtn;

    Clock animClock;
    void centerText(Text& text);

public:
    explicit MainMenuState(PlanetBackground* planet);

    int  update(RenderWindow& window, Event& event) override;
    void updateLogic(RenderWindow& window);
    void render(RenderWindow& window) override;
};