#pragma once
#include "State.h"
#include "Database.h"
#include <string>

class LoginState : public State {
private:
    Font font;
    float cx;

    Text title;

    // Поля
    Text lblLogin, lblPass;
    RectangleShape boxLogin, boxPass;
    Text fldLogin, fldPass;

    // Курсор
    RectangleShape caret;
    Clock caretClock;
    bool caretVisible;

    // Кнопки
    Text btnSubmit, btnBack;

    // Сообщение об ошибке
    Text msgError;

    // Данные
    std::string sLogin, sPass;
    int activeField; // 0 = логин, 1 = пароль

    Database* db;

    void centerText(Text& t);
    void refreshFields();

public:
    LoginState(Database* database);

    int update(RenderWindow& window, Event& event) override;
    void updateLogic(RenderWindow& window);
    void render(RenderWindow& window) override;
};