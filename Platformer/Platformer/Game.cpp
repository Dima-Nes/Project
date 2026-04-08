#include "Game.h" // Подключаем ЗАГОЛОВОК, а не .cpp

Game::Game() {
    // Используем разрешение экрана пользователя, чтобы не было проблем с Full HD
    VideoMode desktop = VideoMode::getDesktopMode();
    window.create(desktop, L"Курсовая работа", Style::Fullscreen);
    window.setFramerateLimit(60);

    menu = new MainMenuState();
    registration = new RegistrationState();
    currentState = 0;
}

Game::~Game() {
    delete menu;
    delete registration;
}

void Game::run() {
    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) window.close();

            // Важно для регистрации: передаем события (текстовый ввод)
            if (currentState == 1) {
                registration->update(window, event);
            }
        }

        window.clear(Color(30, 30, 30));

        if (currentState == 0) {
            // Обновляем меню. Если оно вернуло 1 — переключаем на регистрацию
            if (menu->update(window) == 1) {
                currentState = 1;
            }
            menu->render(window);
        }
        else if (currentState == 1) {
            // В RegistrationState добавь логику, чтобы update возвращал 0 для кнопки "Назад"
            // if (registration->update(window, event) == 0) currentState = 0;
            registration->render(window);
        }

        window.display();
    }
}