#include "HUD.h"
#include <string>

HUD::HUD() {}

void HUD::setupText(Text& t, float x, float y, Color color) {
    t.setFont(font);
    t.setCharacterSize(30);
    t.setFillColor(color);
    t.setOutlineColor(Color(0, 0, 0, 200));
    t.setOutlineThickness(2.f);
    t.setPosition(x, y);
}

bool HUD::loadFont(const std::string& path) {
    if (!font.loadFromFile(path)) return false;
    setupText(txtScore, 24.f, 20.f, Color::White);
    setupText(txtLives, 24.f, 60.f, Color(255, 80, 80));
    return true;
}

void HUD::update(const Player& player) {
    txtScore.setString(L"Очки: " + std::to_wstring(player.getScore()));
    txtLives.setString(L"Жизни: " + std::to_wstring(player.getLives()));
}

void HUD::render(RenderWindow& window) {
    // Временно ставим стандартный вид (экранные координаты), рисуем, возвращаем
    View saved = window.getView();
    window.setView(window.getDefaultView());
    window.draw(txtScore);
    window.draw(txtLives);
    window.setView(saved);
}