#include "Game.h"

Game::Game() {
    VideoMode desktop = VideoMode::getDesktopMode(); // получения разрешения монитора
    window.create(desktop, "Lost Horizons", Style::None);
    window.setFramerateLimit(60);
    gameView.setSize(1920.f, 1080.f);
    gameView.setCenter(1920.f / 2.f, 1080.f / 2.f);

    currentState = GameState::Splash;
}

void Game::run() {
    while (window.isOpen()) {
        handleEvents();
        update();
        render();
    }
}

void Game::handleEvents() {
    Event event;
    while (window.pollEvent(event)) {
        if (event.type == Event::Closed)
            window.close();

        if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
            window.close();
    }
}

void Game::update() {
    // Тут будет логика позже
}

void Game::render() {
    window.clear();
    window.setView(gameView);
    if (currentState == GameState::Splash) {

        window.draw(splashSprite);
    }

    window.display();
}