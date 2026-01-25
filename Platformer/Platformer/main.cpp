#include <SFML/Graphics.hpp>
#include "Database.h"
#include <iostream>

int main() {
    // Включаем поддержку русского языка в консоли (чтобы не было кракозябр)
    setlocale(LC_ALL, "");

    Database db;
    if (!db.connect("GameData.db")) {
        return -1; // Если базы нет, выходим
    }
    db.createTables();

    // Простое меню для теста
    std::cout << "1. Регистрация\n2. Вход\nВыберите действие: ";
    int choice;
    std::cin >> choice;

    std::string login, pass;
    std::cout << "Введите логин: ";
    std::cin >> login;
    std::cout << "Введите пароль: ";
    std::cin >> pass;

    if (choice == 1) {
        db.registerUser(login, pass);
    }
    else if (choice == 2) {
        db.loginUser(login, pass);
    }

    // Пока уберем создание окна SFML, чтобы сосредоточиться на логике
    system("pause");
    return 0;
}