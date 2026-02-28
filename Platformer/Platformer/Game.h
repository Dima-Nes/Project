#pragma once
#include <SFML/Graphics.hpp>
#include "State.h"

using namespace sf;

class Game {
private:
    GameState currentState;
    RenderWindow window;// основное окно программы
    View gameView;//виртуальная камера
    void handleEvents();
    void update();
    void render();

    Texture splashTexture;
    Sprite splashSprite;

    Clock splashTimer; // Секундомер
    float splashAlpha;
public:
    Game();
    ~Game(); // Обязательно добавим деструктор для очистки памяти

    void run();
    void update();
    void render();
};