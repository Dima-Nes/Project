#include "PlayState.h"
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <iostream>

// ─── Конструктор ─────────────────────────────────────────────────────────────

PlayState::PlayState(Database* db, const std::string& username,
    int savedSeed, float spawnX, float spawnY)
    : db(db), username(username)
{
    // Генерируем мир: если передан сохранённый сид — используем его
    int seed = (savedSeed == -1) ? (int)std::time(nullptr) : savedSeed;
    std::cout << "World seed: " << seed << std::endl;
    world.generate(seed);

    if (!player.loadTexture("assets/knight.png"))
        std::cerr << "Warning: knight.png not found.\n";

    // Если переданы сохранённые координаты — ставим игрока туда,
    // иначе — стандартный спавн на поверхности
    if (spawnX >= 0.f && spawnY >= 0.f)
        player.spawnAt(spawnX, spawnY);
    else
        player.spawn(world);

    if (!hud.loadFont("assets/font.ttf"))
        std::cerr << "Warning: font.ttf not found.\n";
    hud.update(player);
}

// ─── Сохранение прогресса ─────────────────────────────────────────────────────

void PlayState::saveProgress() {
    if (!db) return;
    Vector2f pos = player.getCenter();
    db->saveWorld(username, world.getSeed(), pos.x, pos.y);
    db->updateHighScore(username, player.getScore());
}

// ─── Построение меню паузы ───────────────────────────────────────────────────

void PlayState::buildPauseMenu(RenderWindow& window) {
    pauseFont.loadFromFile("assets/font.ttf");

    float W = (float)window.getSize().x;
    float H = (float)window.getSize().y;
    float cx = W / 2.f;

    // Полупрозрачный фон на весь экран
    pauseOverlay.setSize({ W, H });
    pauseOverlay.setPosition(0.f, 0.f);
    pauseOverlay.setFillColor(Color(0, 0, 0, 160));

    pauseTitle.setFont(pauseFont);
    pauseTitle.setString(L"Пауза");
    pauseTitle.setCharacterSize(72);
    pauseTitle.setFillColor(Color::White);
    {
        FloatRect r = pauseTitle.getLocalBounds();
        pauseTitle.setOrigin(r.left + r.width / 2.f, r.top + r.height / 2.f);
    }
    pauseTitle.setPosition(cx, H * 0.33f);

    btnResume.setFont(pauseFont);
    btnResume.setString(L"Продолжить");
    btnResume.setCharacterSize(54);
    btnResume.setFillColor(Color::White);
    {
        FloatRect r = btnResume.getLocalBounds();
        btnResume.setOrigin(r.left + r.width / 2.f, r.top + r.height / 2.f);
    }
    btnResume.setPosition(cx, H * 0.50f);

    btnBackToMenu.setFont(pauseFont);
    btnBackToMenu.setString(L"Выйти в главное меню");
    btnBackToMenu.setCharacterSize(54);
    btnBackToMenu.setFillColor(Color::White);
    {
        FloatRect r = btnBackToMenu.getLocalBounds();
        btnBackToMenu.setOrigin(r.left + r.width / 2.f, r.top + r.height / 2.f);
    }
    btnBackToMenu.setPosition(cx, H * 0.62f);
}

// ─── Обновление меню паузы (события) ─────────────────────────────────────────

int PlayState::updatePauseMenu(RenderWindow& window, Event& event) {
    // Кнопки в паузе рисуются в экранных координатах, поэтому
    // используем стандартные координаты, а не camera view.
    RenderWindow& win = window;

    // Анимация кнопок (hover)
    Vector2i mp = Mouse::getPosition(win);
    Vector2f mouseScreen((float)mp.x, (float)mp.y);

    // Обновляем цвет на hover (вызываем каждый кадр через updateLogic — здесь только события)
    if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape) {
        paused = false; // ESC снова → снять паузу
        return -1;
    }

    if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
        Vector2f m((float)event.mouseButton.x, (float)event.mouseButton.y);

        if (btnResume.getGlobalBounds().contains(m)) {
            paused = false;
            return -1;
        }
        if (btnBackToMenu.getGlobalBounds().contains(m)) {
            saveProgress();
            return 0; // выход в GameMenu
        }
    }
    return -1;
}

// ─── Рендер меню паузы ───────────────────────────────────────────────────────

void PlayState::renderPauseMenu(RenderWindow& window) {
    // Переключаемся на стандартный вид (экранные координаты)
    window.setView(window.getDefaultView());

    // Hover-анимация кнопок
    Vector2f mouse((float)Mouse::getPosition(window).x,
        (float)Mouse::getPosition(window).y);

    float dt = pauseAnimClock.restart().asSeconds();
    Text* btns[] = { &btnResume, &btnBackToMenu };
    for (auto* b : btns) {
        bool  hov = b->getGlobalBounds().contains(mouse);
        Color tgt = hov ? Color::Yellow : Color::White;
        Color cur = b->getFillColor();
        b->setFillColor(Color(
            (sf::Uint8)(cur.r + (tgt.r - cur.r) * 10.f * dt),
            (sf::Uint8)(cur.g + (tgt.g - cur.g) * 10.f * dt),
            (sf::Uint8)(cur.b + (tgt.b - cur.b) * 10.f * dt)
        ));
        float ts = b->getScale().x + ((hov ? 1.1f : 1.f) - b->getScale().x) * 10.f * dt;
        b->setScale(ts, ts);
    }

    window.draw(pauseOverlay);
    window.draw(pauseTitle);
    window.draw(btnResume);
    window.draw(btnBackToMenu);

    // Возвращаем камеру обратно
    window.setView(camera);
}

// ─── Камера ──────────────────────────────────────────────────────────────────

void PlayState::updateCamera(RenderWindow& window) {
    float ww = (float)window.getSize().x;
    float wh = (float)window.getSize().y;

    if (camera.getSize().x == 0) {
        camera.setSize(ww, wh);
        camera.setCenter(player.getCenter());
    }

    float worldW = World::WIDTH * World::TILE_SIZE;
    float worldH = World::HEIGHT * World::TILE_SIZE;

    Vector2f target = player.getCenter();
    float hw = ww / 2.f, hh = wh / 2.f;
    target.x = std::max(hw, std::min(target.x, worldW - hw));
    target.y = std::max(hh, std::min(target.y, worldH - hh));

    Vector2f cur = camera.getCenter();
    camera.setCenter(cur + (target - cur) * 0.12f);
    window.setView(camera);
}

// ─── update (события) ────────────────────────────────────────────────────────

int PlayState::update(RenderWindow& window, Event& event) {
    // Если меню паузы не построено при первом вызове — строим
    static bool pauseBuilt = false;
    if (!pauseBuilt) { buildPauseMenu(window); pauseBuilt = true; }

    if (paused)
        return updatePauseMenu(window, event);

    // ESC → открываем паузу (не выходим сразу)
    if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape) {
        paused = true;
        pauseAnimClock.restart();
        return -1;
    }

    player.handleEvent(event);
    return -1;
}

// ─── updateLogic ─────────────────────────────────────────────────────────────

void PlayState::updateLogic(RenderWindow& window, float dt) {
    if (paused) return; // Физика на паузе не обновляется

    player.update(dt, world);
    hud.update(player);
    updateCamera(window);
}

// ─── render ──────────────────────────────────────────────────────────────────

void PlayState::render(RenderWindow& window) {
    world.render(window, camera);
    player.render(window);
    hud.render(window);

    if (paused)
        renderPauseMenu(window);
}