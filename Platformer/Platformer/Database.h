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
    bool createTables(); // создаёт Users + WorldSaves

    // ─── Аккаунты ─────────────────────────────────────────────────────────────
    bool registerUser(const std::string& username, const std::string& password);
    bool loginUser(const std::string& username, const std::string& password);
    bool updateHighScore(const std::string& username, int score);

    // ─── Мир ──────────────────────────────────────────────────────────────────
    // Сохранить сид и позицию игрока (перезаписывает если уже есть)
    bool saveWorld(const std::string& username, int seed, float playerX, float playerY);

    // Загрузить сохранение. Возвращает false если сохранения нет.
    bool loadWorld(const std::string& username, int& seed, float& playerX, float& playerY);

    // Удалить сохранение (кнопка «Новый мир»)
    bool deleteWorld(const std::string& username);

    // Есть ли сохранённый мир для этого пользователя?
    bool hasWorld(const std::string& username);
};