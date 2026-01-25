#include <SFML/Graphics.hpp>
#include "sqlite3.h"
#include <iostream>

int main() {
    // 1. Проверка SFML
    sf::RenderWindow window(sf::VideoMode(200, 200), "IT WORKS!");
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    // 2. Проверка SQLite
    sqlite3* db;
    int rc = sqlite3_open("test.db", &db);
    if (rc) {
        std::cerr << "Error opening SQLite database: " << sqlite3_errmsg(db) << std::endl;
    }
    else {
        std::cout << "Opened database successfully!" << std::endl;
        sqlite3_close(db);
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        window.clear();
        window.draw(shape);
        window.display();
    }

    return 0;
}