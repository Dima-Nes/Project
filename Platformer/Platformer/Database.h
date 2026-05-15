#pragma once
#include <string>
#include <vector>
#include "sqlite3.h"
#include "EnemyRecord.h"

// ← Вне класса, в глобальном пространстве имён

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

    bool registerUser(const std::string& username, const std::string& password);
    bool loginUser(const std::string& username, const std::string& password);
    bool updateHighScore(const std::string& username, int score);

    bool saveWorld(const std::string& username, int seed, float playerX, float playerY);
    bool loadWorld(const std::string& username, int& seed, float& playerX, float& playerY);
    bool deleteWorld(const std::string& username);
    bool hasWorld(const std::string& username);

    bool saveEnemies(const std::string& username, const std::vector<EnemyRecord>& data);
    bool loadEnemies(const std::string& username, std::vector<EnemyRecord>& data);
    bool hasEnemies(const std::string& username);
};