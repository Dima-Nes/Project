#pragma once
#include <SFML/Graphics.hpp>
using namespace std;
using namespace sf;

enum class GameState {
    Splash,
    Login,
    Menu,
    Playing
};

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


public:
    Game();
    void run();
};
