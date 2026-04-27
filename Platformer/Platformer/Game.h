#pragma once
#include <SFML/Graphics.hpp>
#include "MainMenuState.h"
#include "RegistrationState.h"
#include "SplashState.h"// 1. Обязательно подключаем
#include "LoginState.h"

using namespace sf;

class Game {
private:
    RenderWindow window;
    MainMenuState* menu;
    RegistrationState* registration;
    SplashState* splash; // 2. Добавляем указатель
    LoginState* login;

    int currentState;

public:
    Game();
    ~Game();
    void run();
};