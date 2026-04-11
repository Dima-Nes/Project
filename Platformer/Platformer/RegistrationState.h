#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

using namespace sf;
using namespace std;

class RegistrationState {
private:
    Font mainFont;
    RectangleShape cursor;
    Clock cursorClock;
    bool showCursor = true;
    size_t maxChars = 15;

    Text title;
    Text backBtn, exitBtn;

    struct InputField {
        RectangleShape box;
        Text label;
        Text userInput;
        wstring content; // Используем wstring для русского языка
    };

    InputField fields[3];
    int activeFieldIndex = -1; // Какое поле сейчас выбрано

    void centerText(Text& text);

public:
    RegistrationState();
    int update(RenderWindow& window, Event& event) override;
    void render(RenderWindow& window);
};