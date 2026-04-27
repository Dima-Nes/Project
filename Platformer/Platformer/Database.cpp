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
    std::cout << "Database connection established: " << filename << std::endl;
    return true;
}

void Database::close() {
    if (isConnected && db) {
        sqlite3_close(db);
        db = nullptr;
        isConnected = false;
        std::cout << "Database connection closed." << std::endl;
    }
}

bool Database::createTables() {
    const char* sql = R"(
        CREATE TABLE IF NOT EXISTS Users (
            ID        INTEGER PRIMARY KEY AUTOINCREMENT,
            Username  TEXT    UNIQUE NOT NULL,
            Password  TEXT    NOT NULL,
            HighScore INTEGER DEFAULT 0
        );
    )";
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL Error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    std::cout << "Tables created (or already exist)." << std::endl;
    return true;
}

bool Database::registerUser(const std::string& username, const std::string& password) {
    const char* sql = "INSERT INTO Users (Username, Password) VALUES (?, ?);";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        std::cerr << "Ошибка подготовки запроса: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);
    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    if (success)
        std::cout << "Пользователь " << username << " успешно зарегистрирован!" << std::endl;
    else
        std::cerr << "Ошибка регистрации (возможно, логин занят): " << sqlite3_errmsg(db) << std::endl;
    sqlite3_finalize(stmt);
    return success;
}

bool Database::loginUser(const std::string& username, const std::string& password) {
    const char* sql = "SELECT ID FROM Users WHERE Username = ? AND Password = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) return false;
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);
    bool success = (sqlite3_step(stmt) == SQLITE_ROW);
    if (success)
        std::cout << "Успешный вход! Добро пожаловать, " << username << "." << std::endl;
    else
        std::cout << "Неверный логин или пароль." << std::endl;
    sqlite3_finalize(stmt);
    return success;
}

// ─── НОВОЕ: рекорды ───────────────────────────────────────────────────────────

bool Database::updateHighScore(const std::string& username, int score) {
    // Обновляем только если новый score > старого (через условие в WHERE)
    const char* sql =
        "UPDATE Users SET HighScore = ? WHERE Username = ? AND HighScore < ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) return false;
    sqlite3_bind_int(stmt, 1, score);
    sqlite3_bind_text(stmt, 2, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, score);
    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    if (ok) std::cout << "Рекорд обновлён: " << score << " для " << username << std::endl;
    return ok;
}

int Database::getHighScore(const std::string& username) {
    const char* sql = "SELECT HighScore FROM Users WHERE Username = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) return 0;
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    int score = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW)
        score = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    return score;
}