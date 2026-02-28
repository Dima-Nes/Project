#include "Game.h"
#include "MainMenuState.h"



Game::Game() {
    // Делаем на весь экран монитора
    window.create(VideoMode::getDesktopMode(), "My Game", Style::Fullscreen);
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