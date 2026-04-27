#include "Game.h"

// ─── Таблица состояний ───────────────────────────────────────────────────────
//  -1  SplashState        (заставка)
//   0  MainMenuState      (вход / регистрация / выход)
//   1  RegistrationState  (создание аккаунта)
//   2  LoginState         (вход в аккаунт)
//   3  GameMenuState      (главное меню игры)

Game::Game() {
    VideoMode desktop = VideoMode::getDesktopMode();
    window.create(desktop, L"Курсовая работа", Style::Fullscreen);
    window.setFramerateLimit(60);

    database = new Database();
    database->connect("users.db");
    database->createTables();

    splash = new SplashState();
    menu = new MainMenuState();
    registration = new RegistrationState(database);
    login = new LoginState(database);
    gameMenu = new GameMenuState();

    currentState = -1;
}

Game::~Game() {
    delete splash;
    delete menu;
    delete registration;
    delete login;
    delete gameMenu;
    delete database;
}

void Game::run() {
    Clock frameClock;

    while (window.isOpen()) {
        float dt = frameClock.restart().asSeconds();
        Event event;

        // ── 1. Обрабатываем все события ──────────────────────────────────────
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) window.close();

            if (currentState == -1) {
                if (splash->update(window, event) == 1) currentState = 0;
            }
            else if (currentState == 0) {
                int res = menu->update(window, event);
                if (res == 1) currentState = 2;  // Вход → LoginState
                if (res == 2) currentState = 1;  // Регистрация → RegistrationState
            }
            else if (currentState == 1) {
                int res = registration->update(window, event);
                if (res == 0) currentState = 0;
                if (res == 2) currentState = 3;
            }
            else if (currentState == 2) {
                int res = login->update(window, event);
                if (res == 0) currentState = 0;
                if (res == 2) currentState = 3;
            }
            else if (currentState == 3) {
                int res = gameMenu->update(window, event);
                if (res == 0) window.close();
            }
        }

        // ── 2. Логика каждый кадр (анимации, курсор, физика) ─────────────────
        if (currentState == 0) menu->updateLogic(window);         // ← ИСПРАВЛЕНО
        if (currentState == 1) registration->updateLogic(window);
        if (currentState == 2) login->updateLogic(window);
        if (currentState == 3) gameMenu->updateLogic(window);

        // ── 3. Рендер ────────────────────────────────────────────────────────
        window.clear(Color(30, 30, 30));
        if (currentState == -1) splash->render(window);
        else if (currentState == 0) menu->render(window);
        else if (currentState == 1) registration->render(window);
        else if (currentState == 2) login->render(window);
        else if (currentState == 3) gameMenu->render(window);
        window.display();
    }
}