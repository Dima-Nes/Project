#include "Game.h" // Подключаем ЗАГОЛОВОК, а не .cpp

Game::Game() {
    VideoMode desktop = VideoMode::getDesktopMode();
    window.create(desktop, L"Курсовая работа", Style::Fullscreen);
    window.setFramerateLimit(60);

    // 3. Создаем объект заставки в памяти
    splash = new SplashState();
    menu = new MainMenuState();
    registration = new RegistrationState();

    // 4. МЕНЯЕМ НА -1, чтобы запуск начинался с заставки
    currentState = -1;
}

Game::~Game() {
    delete splash; // 5. Чистим память за собой
    delete menu;
    delete registration;
}

void Game::run() {
    while (window.isOpen()) {
        Event event;

        // 1. СНАЧАЛА ОБРАБАТЫВАЕМ ВСЕ СОБЫТИЯ
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) window.close();

            if (currentState == -1) {
                if (splash->update(window, event) == 1) currentState = 0;
            }
            else if (currentState == 0) {
                if (menu->update(window, event) == 1) currentState = 1;
            }
            else if (currentState == 1) {
                int res = registration->update(window, event); // Только события!
                if (res == 0) currentState = 0;
            }
        }

        // 2. ЗАТЕМ ОБНОВЛЯЕМ ЛОГИКУ АНИМАЦИЙ (вне цикла pollEvent!)
        if (currentState == 1) {
            registration->updateLogic(window); // Курсор будет мигать ВСЕГДА
        }

        // 3. РИСУЕМ
        window.clear(Color(30, 30, 30));
        if (currentState == -1) splash->render(window);
        else if (currentState == 0) menu->render(window);
        else if (currentState == 1) registration->render(window);
        window.display();
    }
}