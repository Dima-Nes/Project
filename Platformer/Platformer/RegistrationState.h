#pragma once
#include "State.h"
#include "Database.h"
#include "PlanetBackground.h"
#include <string>

class RegistrationState : public State {
private:
    PlanetBackground* planet;
    Font  font;
    float cx;

    Text           title;
    Text           lblLogin, lblPass, lblConfirm;
    RectangleShape boxLogin, boxPass, boxConfirm;
    Text           fldLogin, fldPass, fldConfirm;
    RectangleShape caret;
    Clock          caretClock;
    bool           caretVisible = true;

    Text  btnSubmit, btnBack;
    Text  msgError;

    std::string sLogin, sPass, sConfirm;
    int         activeField = 0;

    Database* db;
    Clock     animClock;

    void centerText(Text& t);
    void refreshFields();
    void setError(const wchar_t* msg);
    bool trySubmit();

public:
    RegistrationState(Database* database, PlanetBackground* planet);

    int  update(RenderWindow& window, Event& event) override;
    void updateLogic(RenderWindow& window);
    void render(RenderWindow& window) override;

    const std::string& getLastUsername() const { return sLogin; }
};