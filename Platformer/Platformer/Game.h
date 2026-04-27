#pragma once
#include <SFML/Graphics.hpp>
#include "SplashState.h"
#include "MainMenuState.h"
#include "RegistrationState.h"
#include "LoginState.h"       // ÍÎÂÎÅ
#include "GameMenuState.h"    // ÍÎÂÎÅ
#include "Database.h"         // ÍÎÂÎÅ

using namespace sf;

class Game {
private:
    RenderWindow window;
    int currentState;

    SplashState* splash;
    MainMenuState* menu;
    RegistrationState* registration;
    LoginState* login;       // ÍÎÂÎÅ
    GameMenuState* gameMenu;    // ÍÎÂÎÅ

    Database* database;   // ÍÎÂÎÅ — îäèí îáúåêò íà âñş èãğó

public:
    Game();
    ~Game();
    void run();
};