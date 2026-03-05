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
    FloatRect bounds = authTitle.getLocalBounds();
    authTitle.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
    authTitle.setPosition(1920 / 2.f, 150.f);


    loginBtn.setFont(mainFont);
    loginBtn.setString(L"Вход");
    loginBtn.setCharacterSize(45);
    bounds = loginBtn.getLocalBounds(); //получаем размеры прямоугольника в котором находиться наш текст
    loginBtn.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
    loginBtn.setPosition(1920 / 2.f, 400.f);

    registerBtn.setFont(mainFont);
    registerBtn.setString(L"Регистрация");
    registerBtn.setCharacterSize(45);
    bounds = registerBtn.getLocalBounds();
    registerBtn.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
    registerBtn.setPosition(1920 / 2.f, 500.f);

    exitBtn.setFont(mainFont);
    exitBtn.setString(L"Выход");
    exitBtn.setCharacterSize(45);
    bounds = exitBtn.getLocalBounds();
    exitBtn.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
    exitBtn.setPosition(1920 / 2.f, 600.f);
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

        Text* allButtons[] = { &loginBtn, &registerBtn, &exitBtn }; // Передаем указатели на кнопки что бы не менять их копии
    
        for (Text* btn : allButtons) {
            if (btn->getGlobalBounds().contains(worldPos)) {
                btn->setFillColor(Color::Yellow);
                if (btn->getScale().x < 1.15f) {
                    btn->setScale(btn->getScale().x + 0.01f, btn->getScale().y + 0.01f);
                }
            }
            else {
                btn->setFillColor(Color::White);
                if (btn->getScale().x > 1.0f) {
                    btn->setScale(btn->getScale().x - 0.01f, btn->getScale().y - 0.01f);
                }
            }
        }


    }

    window.display();
}