#pragma once
#include "State.h"
#include "Database.h"
#include <string>

class RegistrationState : public State {
private:
    Font font;
    float cx;

    Text title;

    Text lblLogin, lblPass, lblConfirm;
    RectangleShape boxLogin, boxPass, boxConfirm;
    Text fldLogin, fldPass, fldConfirm;

    RectangleShape caret;
    Clock caretClock;
    bool caretVisible;

    Text btnSubmit, btnBack;
    Text msgError;

    std::string sLogin, sPass, sConfirm;
    int activeField;

    Database* db;

    void centerText(Text& t);
    void refreshFields();
    void setError(const wchar_t* msg);
    bool trySubmit();

public:
    RegistrationState(Database* database);

    int  update(RenderWindow& window, Event& event) override;
    void updateLogic(RenderWindow& window);
    void render(RenderWindow& window) override;

    // ─── НОВОЕ ────────────────────────────────────────────────────────────────
    // Возвращает логин только что созданного аккаунта.
    const std::string& getLastUsername() const { return sLogin; }
};