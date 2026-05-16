#pragma once
#include "State.h"
#include "PlanetBackground.h"

class GameMenuState : public State {
private:
    PlanetBackground* planet;

    Font  font;
    float cx;

    Text title;
    Text btnPlay, btnSettings, btnNewWorld, btnExit;

    Clock animClock;
    void centerText(Text& t);

public:
    explicit GameMenuState(PlanetBackground* planet);

    int  update(RenderWindow& window, Event& event) override;
    void updateLogic(RenderWindow& window);
    void render(RenderWindow& window) override;
};