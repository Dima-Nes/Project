#pragma once
#include <string>
#include "sqlite3.h"

class Database {
private:
    sqlite3* db;
    bool isConnected;

public:
    Database();
    ~Database();

    bool connect(const std::string& filename);
    void close();
    bool createTables();

    // Пытается создать нового пользователя. Вернёт false, если логин занят.
    bool registerUser(const std::string& username, const std::string& password);

    // Проверяет логин и пароль. Вернёт true при совпадении.
    bool loginUser(const std::string& username, const std::string& password);

    // ─── НОВОЕ: рекорды ───────────────────────────────────────────────────────
    // Обновляет HighScore только если новый score > текущего.
    bool updateHighScore(const std::string& username, int score);

    // Возвращает текущий рекорд пользователя (0 если не найден).
    int  getHighScore(const std::string& username);
};