#include "Database.h"
#include <iostream>

Database::Database() : db(nullptr), isConnected(false) {}

Database::~Database() {
    close();
}

bool Database::connect(const std::string& filename) {
    // Пытаемся открыть файл базы данных
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
    // SQL-запрос для создания таблицы пользователей
    // Храним: ID, Логин (уникальный), Пароль, Рекорд очков
    const char* sql = R"(
        CREATE TABLE IF NOT EXISTS Users (
            ID INTEGER PRIMARY KEY AUTOINCREMENT,
            Username TEXT UNIQUE NOT NULL,
            Password TEXT NOT NULL,
            HighScore INTEGER DEFAULT 0
        );
    )";

    char* errMsg = nullptr;
    // Выполняем SQL запрос
    int rc = sqlite3_exec(db, sql, 0, 0, &errMsg);

    if (rc != SQLITE_OK) {
        std::cerr << "SQL Error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }

    std::cout << "Tables created (or already exist)." << std::endl;
    return true;
}

// ... (предыдущий код оставляем, добавляем это в конец) ...

bool Database::registerUser(const std::string& username, const std::string& password) {
    // 1. Готовим шаблон запроса. Знаки '?' — это места для подстановки данных.
    // INSERT INTO — команда "Вставить внутрь"
    const char* sql = "INSERT INTO Users (Username, Password) VALUES (?, ?);";

    sqlite3_stmt* stmt; // stmt = statement (подготовленное выражение)

    // 2. Компилируем запрос (проверяем, нет ли ошибок в SQL-синтаксисе)
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        std::cerr << "Ошибка подготовки запроса: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    // 3. Привязываем данные к знакам вопроса
    // (1 - это первый знак ?, 2 - это второй знак ?)
    // SQLITE_STATIC означает, что текст не нужно копировать, мы гарантируем, что он существует
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);

    // 4. Выполняем шаг (Step). Для INSERT это и есть выполнение.
    bool success = false;
    if (sqlite3_step(stmt) == SQLITE_DONE) {
        success = true;
        std::cout << "Пользователь " << username << " успешно зарегистрирован!" << std::endl;
    }
    else {
        // Скорее всего, ошибка "UNIQUE constraint failed" (такой логин уже есть)
        std::cerr << "Ошибка регистрации (возможно, логин занят): " << sqlite3_errmsg(db) << std::endl;
    }

    // 5. Обязательно удаляем запрос из памяти
    sqlite3_finalize(stmt);
    return success;
}

bool Database::loginUser(const std::string& username, const std::string& password) {
    // SELECT * — выбрать всё
    // WHERE — где ...
    const char* sql = "SELECT ID FROM Users WHERE Username = ? AND Password = ?;";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);

    bool success = false;
    // Если sqlite3_step вернул SQLITE_ROW, значит нашлась строка в таблице (пользователь существует)
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        success = true;
        std::cout << "Успешный вход! Добро пожаловать, " << username << "." << std::endl;
    }
    else {
        std::cout << "Неверный логин или пароль." << std::endl;
    }

    sqlite3_finalize(stmt);
    return success;
}