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
    PlayState* play;

    Database* database;

    Clock frameClock;

    void startPlay(bool forceNew); // Играть / Новый мир

public:
    Game();
    ~Game();
    void run();
};