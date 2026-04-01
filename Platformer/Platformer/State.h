#pragma once
#include <SFML/Graphics.hpp>

using namespace std; // Как ты и привык
using namespace sf;

class State {
public:
    virtual ~State() {}

    // Эти функции обязаны быть у каждого окна
    virtual void update(RenderWindow& window) = 0;
    virtual void render(RenderWindow& window) = 0;
};