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

    // База данных — создаём один раз, передаём в нужные экраны
    database = new Database();
    database->connect("users.db");
    database->createTables();

    splash = new SplashState();
    menu = new MainMenuState();
    registration = new RegistrationState(database);  // передаём БД
    login = new LoginState(database);          // передаём БД
    gameMenu = new GameMenuState();

    currentState = -1;
}

Game::~Game() {
    delete splash;
    delete menu;
    delete registration;
    delete login;
    delete gameMenu;
    delete database;   // закрывает соединение в деструкторе Database
}

void Game::run() {
    while (window.isOpen()) {
        Event event;

        // ── 1. Обрабатываем все события ──────────────────────────────────────
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) window.close();

            if (currentState == -1) {
                if (splash->update(window, event) == 1) currentState = 0;
            }
            else if (currentState == 0) {
                int res = menu->update(window, event);
                if (res == 1) currentState = 2;  // кнопка «Вход» → LoginState
                if (res == 2) currentState = 1;  // кнопка «Регистрация» → RegistrationState
            }
            else if (currentState == 1) {        // Регистрация
                int res = registration->update(window, event);
                if (res == 0) currentState = 0;  // назад в меню
                if (res == 2) currentState = 3;  // успех → GameMenu
            }
            else if (currentState == 2) {        // Вход
                int res = login->update(window, event);
                if (res == 0) currentState = 0;  // назад в меню
                if (res == 2) currentState = 3;  // успех → GameMenu
            }
            else if (currentState == 3) {        // Игровое меню
                int res = gameMenu->update(window, event);
                if (res == 0) window.close();    // выйти из игры
                // res == 1 (Играть), 2 (Настройки), 3 (Новый мир) — реализуем позже
            }
        }

        // ── 2. Обновляем логику (анимации/курсор — вне цикла событий!) ───────
        if (currentState == 1) registration->updateLogic(window);
        if (currentState == 2) login->updateLogic(window);
        if (currentState == 3) gameMenu->updateLogic(window);

        // ── 3. Рисуем ────────────────────────────────────────────────────────
        window.clear(Color(30, 30, 30));
        if (currentState == -1) splash->render(window);
        else if (currentState == 0) menu->render(window);
        else if (currentState == 1) registration->render(window);
        else if (currentState == 2) login->render(window);
        else if (currentState == 3) gameMenu->render(window);
        window.display();
    }
}