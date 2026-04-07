#include "Game.h"
#include "MainMenuState.h"

Game::Game() {
    // Делаем на весь экран монитора
    window.create(VideoMode::getDesktopMode(), "My Game", Style::Fullscreen);
    window.setFramerateLimit(60);

    currentState = new MainMenuState();
}

Game::~Game() {
    if (currentState) delete currentState;
}

void Game::update() {
    Event event;
    while (window.pollEvent(event)) {
        if (event.type == Event::Closed)
            window.close();

        // Закрытие на Escape (в Fullscreen это важно)
        if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
            window.close();
    }

    if (currentState) {
        currentState->update(window);
    }
}

void Game::render() {
    window.clear(Color::Black); // ОБЯЗАТЕЛЬНО: очищаем экран перед рисованием
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