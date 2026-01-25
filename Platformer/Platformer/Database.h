#pragma once
#include <string>
#include "sqlite3.h"

class Database {
private:
    sqlite3* db; // Сам "движок" базы данных
    bool isConnected;

public:
    Database();
    ~Database();

    bool connect(const std::string& filename);
    void close();
    bool createTables();

    // Пытается создать нового пользователя. Вернет false, если такой логин уже занят.
    bool registerUser(const std::string& username, const std::string& password);

    // Проверяет, есть ли такой логин с таким паролем.
    bool loginUser(const std::string& username, const std::string& password);
};