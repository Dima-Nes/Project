#include "Game.h"
#include <ctime>
#include <fstream>
#include <iostream>

// ─── Читаем индекс планеты из файла и сразу пишем следующий ─────────────────

int Game::loadAndIncrementPlanetIdx() {
    int idx = 0;
    {
        std::ifstream f("planet_idx.txt");
        if (f.is_open()) f >> idx;
    }
    idx = ((idx % 9) + 9) % 9;     // гарантируем 0..8
    {
        std::ofstream f("planet_idx.txt");
        f << (idx + 1) % 9;         // следующий запуск → следующая планета
    }
    return idx;
}

// ─── Конструктор ─────────────────────────────────────────────────────────────

Game::Game() {
    std::srand((unsigned)std::time(nullptr));

    VideoMode desktop = VideoMode::getDesktopMode();
    window.create(desktop, L"Курсовая работа", Style::Fullscreen);
    window.setFramerateLimit(60);

    // ── Планета ──────────────────────────────────────────────────────────────
    int planetIdx = loadAndIncrementPlanetIdx();
    planet = new PlanetBackground();
    if (!planet->load(planetIdx))
        std::cerr << "Game: planet " << planetIdx << " not loaded\n";

    // ── База данных ───────────────────────────────────────────────────────────
    database = new Database();
    database->connect("users.db");
    database->createTables();

    // ── Состояния (все получают указатель на планету) ─────────────────────────
    splash = new SplashState();
    menu = new MainMenuState(planet);
    registration = new RegistrationState(database, planet);
    login = new LoginState(database, planet);
    gameMenu = new GameMenuState(planet);
    play = nullptr;

    currentState = -1;
}

// ─── Деструктор ──────────────────────────────────────────────────────────────

Game::~Game() {
    delete splash;
    delete menu;
    delete registration;
    delete login;
    delete gameMenu;
    delete play;
    delete planet;
    delete database;
}

// ─── startPlay ───────────────────────────────────────────────────────────────

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

// ─── run ─────────────────────────────────────────────────────────────────────

void Game::run() {
    while (window.isOpen()) {
        float dt = frameClock.restart().asSeconds();
        dt = std::min(dt, 1.f / 30.f);
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
                if (res == 1) startPlay(false);
                if (res == 3) startPlay(true);
            }
            else if (currentState == 4) {
                int res = play->update(window, event);
                if (res == 0) {
                    window.setView(window.getDefaultView());
                    currentState = 3;
                }
            }
        }

        if (currentState == 0) menu->updateLogic(window);
        if (currentState == 1) registration->updateLogic(window);
        if (currentState == 2) login->updateLogic(window);
        if (currentState == 3) gameMenu->updateLogic(window);
        if (currentState == 4 && play) play->updateLogic(window, dt);

        window.clear(Color(18, 18, 24));
        if (currentState == -1) splash->render(window);
        else if (currentState == 0) menu->render(window);
        else if (currentState == 1) registration->render(window);
        else if (currentState == 2) login->render(window);
        else if (currentState == 3) gameMenu->render(window);
        else if (currentState == 4 && play) play->render(window);
        window.display();
    }
}