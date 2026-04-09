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

        // 1. ОТЛАВЛИВАЕМ СОБЫТИЯ (только ввод)
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) 
                window.close();
        }

        // 2. ОБНОВЛЯЕМ ЛОГИКУ (ВНЕ цикла событий!)
        // Мы вызываем update и смотрим, что он вернул (nextState)
        if (currentState == -1) { // Заставка
            if (splash->update(window, event) == 1) { 
                currentState = 0; // Если Splash вернул 1, идем в меню
            }
        } 
        else if (currentState == 0) { // Меню
            if (menu->update(window, event) == 1) { 
                currentState = 1; // Если Меню вернуло 1, идем в регистрацию
            }
        }
        else if (currentState == 1) { // Регистрация
             registration->update(window, event);
             // Здесь потом добавишь возврат в меню
        }

        // 3. РИСУЕМ (ВНЕ цикла событий!)
        window.clear(Color(30, 30, 30));

        if (currentState == -1) splash->render(window);
        else if (currentState == 0) menu->render(window);
        else if (currentState == 1) registration->render(window);

        window.display();
    }
}