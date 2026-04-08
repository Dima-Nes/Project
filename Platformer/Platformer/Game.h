#pragma once
#include <SFML/Graphics.hpp>
#include "MainMenuState.h"
#include "RegistrationState.h"

using namespace sf;

class Game {
private:
    RenderWindow window;
    MainMenuState* menu;
    RegistrationState* registration;

    // 0 - Menu, 1 - Registration
    int currentState;

public:
    Game();
    ~Game();
    void run();
};