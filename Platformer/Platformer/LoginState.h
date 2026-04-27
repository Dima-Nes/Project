#pragma once
#include "State.h"
#include "Database.h"
#include <string>

class LoginState : public State {
private:
    Font font;
    float cx;

    Text title;

    Text lblLogin, lblPass;
    RectangleShape boxLogin, boxPass;
    Text fldLogin, fldPass;

    RectangleShape caret;
    Clock caretClock;
    bool caretVisible;

    Text btnSubmit, btnBack;
    Text msgError;

    std::string sLogin, sPass;
    int activeField;

    Database* db;

    void centerText(Text& t);
    void refreshFields();

public:
    LoginState(Database* database);

    int  update(RenderWindow& window, Event& event) override;
    void updateLogic(RenderWindow& window);
    void render(RenderWindow& window) override;

    // ─── НОВОЕ ────────────────────────────────────────────────────────────────
    // Возвращает логин последнего успешного входа.
    // Game читает его сразу после update() == 2.
    const std::string& getLastUsername() const { return sLogin; }
};