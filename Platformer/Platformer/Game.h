#pragma once
#include <SFML/Graphics.hpp>
#include "SplashState.h"
#include "MainMenuState.h"
#include "RegistrationState.h"
#include "LoginState.h"
#include "GameMenuState.h"
#include "PlayState.h"
#include "Database.h"
#include "PlanetBackground.h"

using namespace sf;

class Game {
private:
    RenderWindow window;
    int currentState;

    PlanetBackground* planet;   // ← одна планета на все меню

    SplashState* splash;
    MainMenuState* menu;
    RegistrationState* registration;
    LoginState* login;
    GameMenuState* gameMenu;
    PlayState* play;

    Database* database;
    Clock       frameClock;

    void startPlay(bool forceNew);
    int  loadAndIncrementPlanetIdx(); // читает/пишет planet_idx.txt

public:
    Game();
    ~Game();
    void run();
};