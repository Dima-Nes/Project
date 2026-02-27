#include "Game.h"
#include <iostream>
//Все функции обновляются 60 раз в секунду, поэтому я не использовал любые временные переменные,
// кроме тех, которые получают значение от других и сразу передают его


Game::Game() {
    VideoMode desktop = VideoMode::getDesktopMode(); // получения разрешения монитора
    window.create(desktop, "Lost Horizons", Style::None);
    window.setFramerateLimit(60);
    gameView.setSize(1920.f, 1080.f);
    gameView.setCenter(1920.f / 2.f, 1080.f / 2.f);

    currentState = GameState::Splash;
    splashTexture.loadFromFile("assets/background.png");
    splashSprite.setTexture(splashTexture);
    splashSprite.setColor(Color(255, 255, 255, 0));

    //Кнопки:
    mainFont.loadFromFile("assets/Genesis.otf");
    authTitle.setFont(mainFont);
    authTitle.setString("LOST HORIZONS");
    authTitle.setCharacterSize(60);
    authTitle.setPosition(1920 / 2.f - 400.f, 150.f);


    loginBtn.setFont(mainFont);
    loginBtn.setString(L"Вход");
    loginBtn.setCharacterSize(45);
    loginBtn.setPosition(1920 / 2.f - 100.f, 400.f);

    registerBtn.setFont(mainFont);
    registerBtn.setString("Регистрация");
    registerBtn.setCharacterSize(45);
    registerBtn.setPosition(1920 / 2.f - 100.f, 500.f);

    exitBtn.setFont(mainFont);
    exitBtn.setString("Выход");
    exitBtn.setCharacterSize(45);
    exitBtn.setPosition(1920 / 2.f - 100.f, 600.f);
}

void Game::run() {
    while (window.isOpen()) {//это строка заставляет все функции выполняться 60 раз в секунду
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
    if (currentState == GameState::Splash) {
        float timePassed = splashTimer.getElapsedTime().asSeconds();

        if (timePassed < 3.0f) {
            splashTransparency = (timePassed / 3.0f) * 255.0f;
            splashSprite.setColor(Color(255, 255, 255, (int)splashTransparency));//(int) - это явное приведение типов, просто отбрасывает все после запятой
        }
        else {
            currentState = GameState::Login;
        }
    }
}

void Game::render() {
    window.setView(gameView);

    if (currentState == GameState::Splash) {
        window.clear();
        window.draw(splashSprite);
    }

    if (currentState == GameState::Login) {
        window.clear(Color(20, 20, 60));
        window.draw(authTitle);
        window.draw(loginBtn);
        window.draw(registerBtn);
        window.draw(exitBtn);

        Vector2i mousePos = Mouse::getPosition(window);
        Vector2f worldPos = window.mapPixelToCoords(mousePos); // Переводим пиксели в координаты мира

        // Проверка для кнопки Login
        if (loginBtn.getGlobalBounds().contains(worldPos)) {
            loginBtn.setFillColor(Color::Yellow); // Подсвечиваем
            // Плавное увеличение
            if (loginBtn.getScale().x < 1.2f) {
                loginBtn.setScale(loginBtn.getScale().x + 0.01f, loginBtn.getScale().y + 0.01f);
            }
        }
        else {
            loginBtn.setFillColor(Color::White);
            // Возвращаем размер обратно
            if (loginBtn.getScale().x > 1.0f) {
                loginBtn.setScale(loginBtn.getScale().x - 0.01f, loginBtn.getScale().y - 0.01f);
            }
        }


    }

    window.display();
}