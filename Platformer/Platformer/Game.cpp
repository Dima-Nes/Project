#include "Game.h"
#include "MainMenuState.h" // Подключаем конкретное меню

Game::Game() {
    window.create(VideoMode(1920, 1080), "My Game");
    window.setFramerateLimit(60);

    // При старте создаем главное меню
    currentState = new MainMenuState();
}

Game::~Game() {
    delete currentState; // Чистим память при выходе
}

void Game::update() {
    Event event;
    while (window.pollEvent(event)) {
        if (event.type == Event::Closed)
            window.close();
    }

    // Передаем работу текущему состоянию
    if (currentState) {
        currentState->update(window);
    }
}

void Game::render() {
    window.clear();

    // Рисуем то, что велит текущее состояние
    if (currentState) {
        currentState->render(window);
    }

    window.display();
}

void Game::run() {
    while (window.isOpen()) {
        update();
        render();
    }
}