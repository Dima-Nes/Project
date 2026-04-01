#pragma once
#include <SFML/Graphics.hpp>
#include "State.h"

using namespace sf;

class Game {
private:
    RenderWindow window;
    State* currentState; // Тот самый "слот" для картриджа

public:
    Game();
    ~Game(); // Обязательно добавим деструктор для очистки памяти

    void run();
    void update();
    void render();
};