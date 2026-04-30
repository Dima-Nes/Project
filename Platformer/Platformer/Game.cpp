#include "Game.h"

// ─── Таблица состояний ───────────────────────────────────────────────────────
//  -1  SplashState
//   0  MainMenuState      (вход / регистрация / выход)
//   1  RegistrationState
//   2  LoginState
//   3  GameMenuState
//   4  PlayState

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
    play = nullptr;

    currentState = -1;
}

Game::~Game() {
    delete splash;
    delete menu;
    delete registration;
    delete login;
    delete gameMenu;
    delete play;
    delete database;
}

// ─── Запуск игры ─────────────────────────────────────────────────────────────

void Game::startPlay(bool forceNew) {
    if (forceNew)
        database->deleteWorld("player");

    delete play;

    int   savedSeed = -1;
    float spawnX = -1.f;
    float spawnY = -1.f;

    if (!forceNew)
        database->loadWorld("player", savedSeed, spawnX, spawnY);

    play = new PlayState(database, "player", savedSeed, spawnX, spawnY);
    currentState = 4;
}

// ─── Главный цикл ────────────────────────────────────────────────────────────

void Game::run() {
    while (window.isOpen()) {
        float dt = frameClock.restart().asSeconds();
        Event event;

        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) window.close();

            if (currentState == -1) {
                if (splash->update(window, event) == 1) currentState = 0;
            }
            else if (currentState == 0) {
                int res = menu->update(window, event);
                if (res == 1) currentState = 2;
                if (res == 2) currentState = 1;
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
                if (res == 1) startPlay(false);   // Играть  → загрузить сохранение
                if (res == 3) startPlay(true);    // Новый мир → удалить и создать новый
            }
            else if (currentState == 4) {
                int res = play->update(window, event);
                if (res == 0) {
                    // ── ВАЖНО: сбрасываем вид на дефолтный перед возвратом в меню ──
                    window.setView(window.getDefaultView());
                    currentState = 3;
                }
            }
        }

        if (currentState == 1) registration->updateLogic(window);
        if (currentState == 2) login->updateLogic(window);
        if (currentState == 3) gameMenu->updateLogic(window);
        if (currentState == 4 && play) play->updateLogic(window, dt);

        window.clear(Color(30, 30, 30));
        if (currentState == -1) splash->render(window);
        else if (currentState == 0) menu->render(window);
        else if (currentState == 1) registration->render(window);
        else if (currentState == 2) login->render(window);
        else if (currentState == 3) gameMenu->render(window);
        else if (currentState == 4 && play) play->render(window);
        window.display();//
    }
}