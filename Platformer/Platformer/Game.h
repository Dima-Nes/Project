#pragma once
#include <SFML/Graphics.hpp>
#include "SplashState.h"
#include "MainMenuState.h"
#include "RegistrationState.h"
#include "LoginState.h"
#include "GameMenuState.h"
#include "PlayState.h"
#include "Database.h"

using namespace sf;

class Game {
private:
    RenderWindow window;
    int currentState;

    SplashState* splash;
    MainMenuState* menu;
    RegistrationState* registration;
    LoginState* login;
    GameMenuState* gameMenu;
    PlayState* play;       // Создаётся при нажатии «Играть», может быть nullptr

    Database* database;

    Clock frameClock; // Для передачи dt в PlayState::updateLogic

public:
    Game();
    ~Game();
    void run();
};