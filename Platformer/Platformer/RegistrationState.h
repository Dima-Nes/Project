#pragma once
#include "State.h"
#include "Database.h"
#include <string>

class RegistrationState : public State {
private:
    Font font;
    float cx;

    Text title;

    // Подписи полей
    Text lblLogin, lblPass, lblConfirm;

    // Рамки полей ввода
    RectangleShape boxLogin, boxPass, boxConfirm;

    // Отображаемый текст в полях
    Text fldLogin, fldPass, fldConfirm;

    // Мигающий курсор
    RectangleShape caret;
    Clock caretClock;
    bool caretVisible;

    // Кнопки
    Text btnSubmit, btnBack;

    // Сообщение об ошибке
    Text msgError;

    // Введённые данные
    std::string sLogin, sPass, sConfirm;
    int activeField; // 0 = логин, 1 = пароль, 2 = подтверждение

    Database* db;

    void centerText(Text& t);
    void refreshFields();
    void setError(const wchar_t* msg);
    bool trySubmit();

public:
    RegistrationState(Database* database);

    int update(RenderWindow& window, Event& event) override;
    void updateLogic(RenderWindow& window); // Курсор и анимации
    void render(RenderWindow& window) override;
};