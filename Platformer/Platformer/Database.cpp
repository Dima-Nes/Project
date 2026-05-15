#include "Database.h"
#include <iostream>

Database::Database() : db(nullptr), isConnected(false) {}

Database::~Database() {
    close();
}

bool Database::connect(const std::string& filename) {
    int rc = sqlite3_open(filename.c_str(), &db);
    if (rc) {
        std::cerr << "Error opening database: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    isConnected = true;
    std::cout << "Database connected: " << filename << std::endl;
    return true;
}

void Database::close() {
    if (isConnected && db) {
        sqlite3_close(db);
        db = nullptr;
        isConnected = false;
    }
}

bool Database::createTables() {
    const char* sql = R"(
        CREATE TABLE IF NOT EXISTS Users (
            ID       INTEGER PRIMARY KEY AUTOINCREMENT,
            Username TEXT UNIQUE NOT NULL,
            Password TEXT NOT NULL,
            HighScore INTEGER DEFAULT 0
        );
        CREATE TABLE IF NOT EXISTS WorldSaves (
            Username TEXT PRIMARY KEY,
            Seed     INTEGER NOT NULL,
            PlayerX  REAL    DEFAULT 0,
            PlayerY  REAL    DEFAULT 0
        );
        CREATE TABLE IF NOT EXISTS EnemyPositions (
            Username TEXT NOT NULL,
            Idx      INTEGER NOT NULL,
            Type     INTEGER NOT NULL,
            X        REAL NOT NULL,
            Y        REAL NOT NULL,
            PRIMARY KEY (Username, Idx)
        );
    )";



    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL Error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

// ─── Аккаунты ────────────────────────────────────────────────────────────────

bool Database::registerUser(const std::string& username, const std::string& password) {
    const char* sql = "INSERT INTO Users (Username, Password) VALUES (?, ?);";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) return false;
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);
    bool ok = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return ok;
}

bool Database::loginUser(const std::string& username, const std::string& password) {
    const char* sql = "SELECT ID FROM Users WHERE Username = ? AND Password = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) return false;
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);
    bool ok = sqlite3_step(stmt) == SQLITE_ROW;
    sqlite3_finalize(stmt);
    return ok;
}

bool Database::updateHighScore(const std::string& username, int score) {
    const char* sql =
        "UPDATE Users SET HighScore = MAX(HighScore, ?) WHERE Username = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) return false;
    sqlite3_bind_int(stmt, 1, score);
    sqlite3_bind_text(stmt, 2, username.c_str(), -1, SQLITE_STATIC);
    bool ok = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return ok;
}

// ─── Мир ──────────────────────────────────────────────────────────────────────

bool Database::saveWorld(const std::string& username, int seed,
    float playerX, float playerY)
{
    // INSERT OR REPLACE — создаёт запись или перезаписывает если уже есть
    const char* sql =
        "INSERT OR REPLACE INTO WorldSaves (Username, Seed, PlayerX, PlayerY) "
        "VALUES (?, ?, ?, ?);";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) return false;
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, seed);
    sqlite3_bind_double(stmt, 3, (double)playerX);
    sqlite3_bind_double(stmt, 4, (double)playerY);
    bool ok = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    if (ok) std::cout << "World saved. Seed=" << seed << " pos=("
        << playerX << "," << playerY << ")\n";
    return ok;
}

bool Database::loadWorld(const std::string& username,
    int& seed, float& playerX, float& playerY)
{
    const char* sql =
        "SELECT Seed, PlayerX, PlayerY FROM WorldSaves WHERE Username = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) return false;
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    bool ok = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        seed = sqlite3_column_int(stmt, 0);
        playerX = (float)sqlite3_column_double(stmt, 1);
        playerY = (float)sqlite3_column_double(stmt, 2);
        ok = true;
        std::cout << "World loaded. Seed=" << seed << " pos=("
            << playerX << "," << playerY << ")\n";
    }
    sqlite3_finalize(stmt);
    return ok;
}

bool Database::deleteWorld(const std::string& username) {
    const char* sql = "DELETE FROM WorldSaves WHERE Username = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) return false;
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    bool ok = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return ok;
}

bool Database::hasWorld(const std::string& username) {
    const char* sql = "SELECT 1 FROM WorldSaves WHERE Username = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) return false;
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    bool found = sqlite3_step(stmt) == SQLITE_ROW;
    sqlite3_finalize(stmt);
    return found;
}
bool Database::saveEnemies(const std::string& username,
    const std::vector<EnemyRecord>& data)
{
    // удаляем старые
    sqlite3_stmt* s;
    sqlite3_prepare_v2(db,
        "DELETE FROM EnemyPositions WHERE Username=?;", -1, &s, 0);
    sqlite3_bind_text(s, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_step(s); sqlite3_finalize(s);

    const char* sql =
        "INSERT INTO EnemyPositions (Username,Idx,Type,X,Y) VALUES(?,?,?,?,?);";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) return false;
    for (int i = 0; i < (int)data.size(); ++i) {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, i);
        sqlite3_bind_int(stmt, 3, data[i].type);
        sqlite3_bind_double(stmt, 4, data[i].x);
        sqlite3_bind_double(stmt, 5, data[i].y);
        sqlite3_step(stmt);
        sqlite3_reset(stmt);
    }
    sqlite3_finalize(stmt);
    return true;
}

bool Database::loadEnemies(const std::string& username,
    std::vector<EnemyRecord>& data)
{
    data.clear();
    const char* sql =
        "SELECT Type,X,Y FROM EnemyPositions WHERE Username=? ORDER BY Idx;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) return false;
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        EnemyRecord r;
        r.type = sqlite3_column_int(stmt, 0);
        r.x = (float)sqlite3_column_double(stmt, 1);
        r.y = (float)sqlite3_column_double(stmt, 2);
        data.push_back(r);
    }
    sqlite3_finalize(stmt);
    return !data.empty();
}

bool Database::hasEnemies(const std::string& username) {
    const char* sql = "SELECT 1 FROM EnemyPositions WHERE Username=? LIMIT 1;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) return false;
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    bool found = sqlite3_step(stmt) == SQLITE_ROW;
    sqlite3_finalize(stmt); return found;
}