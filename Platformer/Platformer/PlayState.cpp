#include "PlayState.h"
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <iostream>



// ─── Конструктор ─────────────────────────────────────────────────────────────

PlayState::PlayState(Database* db, const std::string& username,
    int savedSeed, float spawnX, float spawnY)
    : db(db), username(username), paused(false)
{
    int seed = (savedSeed == -1) ? (int)std::time(nullptr) : savedSeed;
    std::cout << "World seed: " << seed << std::endl;
    world.generate(seed);
    bool texOk = world.loadTextures("assets/");
    std::cout << "Textures loaded: " << texOk << std::endl;
    world.loadTextures("assets/");
    background.load("assets/bg", "assets/bg/Cloud Pack");
    camera = View(); // сброс в дефолт — размер станет 0, initCamera сработает

    if (!player.loadTexture("assets/adventurer.png"))
        std::cerr << "Warning: adventurer.png not found.\n";

    if (spawnX >= 0.f && spawnY >= 0.f)
        player.spawnAt(spawnX, spawnY, world);
    else
        player.spawn(world);

    // Вне if-else — вызывается всегда
    enemyManager.generate(world, username, db);

    if (!hud.loadFont("assets/font.ttf"))
        std::cerr << "Warning: font.ttf not found.\n";
    hud.update(player);

    if (!hud.loadFont("assets/font.ttf"))
        std::cerr << "Warning: font.ttf not found.\n";
    hud.update(player);

    std::cout << "spawnX=" << spawnX << " spawnY=" << spawnY << std::endl;
}

// ─── Сохранение прогресса ────────────────────────────────────────────────────

void PlayState::saveProgress() {
    if (!db) return;
    Vector2f pos = player.getCenter();
    db->saveWorld(username, world.getSeed(), pos.x, pos.y);
    db->updateHighScore(username, player.getScore());
    enemyManager.savePositions(username, db);   // ← добавить
    std::cout << "Progress saved.\n";
}

// ─── Построение меню паузы ───────────────────────────────────────────────────
// Вызывается один раз при первом update — когда window уже точно открыто.

void PlayState::buildPauseMenu(RenderWindow& window) {
    if (pauseBuilt) return; // ← обычный bool, не static
    pauseBuilt = true;

    pauseFont.loadFromFile("assets/font.ttf");

    float W = (float)window.getSize().x;
    float H = (float)window.getSize().y;
    float cx = W / 2.f;

    pauseOverlay.setSize({ W, H });
    pauseOverlay.setPosition(0.f, 0.f);
    pauseOverlay.setFillColor(Color(0, 0, 0, 160));

    auto setupText = [&](Text& t, const wchar_t* s, int size, float y) {
        t.setFont(pauseFont);
        t.setString(s);
        t.setCharacterSize(size);
        t.setFillColor(Color::White);
        FloatRect r = t.getLocalBounds();
        t.setOrigin(r.left + r.width / 2.f, r.top + r.height / 2.f);
        t.setPosition(cx, y);
        };

    setupText(pauseTitle, L"Пауза", 72, H * 0.33f);
    setupText(btnResume, L"Продолжить", 54, H * 0.50f);
    setupText(btnBackToMenu, L"Выйти в главное меню", 54, H * 0.62f);
}

// ─── Обновление меню паузы (события) ────────────────────────────────────────

int PlayState::updatePauseMenu(RenderWindow& window, Event& event) {
    if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape) {
        paused = false;
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
            return 0; // → GameMenu
        }
    }
    return -1;
}

// ─── Рендер меню паузы ───────────────────────────────────────────────────────

void PlayState::renderPauseMenu(RenderWindow& window) {
    // Рисуем поверх мира в экранных координатах
    window.setView(window.getDefaultView());

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

    // Возвращаем вид камеры обратно (для мира)
    window.setView(camera);
}

// ─── Камера ──────────────────────────────────────────────────────────────────

void PlayState::updateCamera(RenderWindow& window) {
    float ww = (float)window.getSize().x;
    float wh = (float)window.getSize().y;

    if (camera.getSize().x < 1.f) {
        camera.setSize(ww, wh);
        camera.setCenter(player.getCenter());
    }

    float worldW = World::WIDTH * World::TILE_SIZE;
    float worldH = World::HEIGHT * World::TILE_SIZE;
    float hw = ww / 2.f, hh = wh / 2.f;

    Vector2f target = player.getCenter();

    // Зажимаем ЦЕЛЬ камеры строго в границах
    float clampedX = std::max(hw, std::min(target.x, worldW - hw));
    float clampedY = std::max(hh, std::min(target.y, worldH - hh));

    Vector2f cur = camera.getCenter();

    // У границ — мгновенный снэп, внутри мира — плавный lerp
    float lerpX = (clampedX <= hw || clampedX >= worldW - hw) ? 1.f : 0.12f;
    float lerpY = (clampedY <= hh || clampedY >= worldH - hh) ? 1.f : 0.12f;

    camera.setCenter(
        cur.x + (clampedX - cur.x) * lerpX,
        cur.y + (clampedY - cur.y) * lerpY
    );
    window.setView(camera);
}

// ─── update (события) ────────────────────────────────────────────────────────

int PlayState::update(RenderWindow& window, Event& event) {
    buildPauseMenu(window); // Строим один раз (внутри есть защита)

    // Экран смерти
    if (deathScreenActive) {
        if (event.type == Event::KeyPressed ||
            event.type == Event::MouseButtonPressed) {
            deathScreenActive = false;
            player.respawn(world);
            window.setView(camera);
        }
        return -1;
    }

    if (paused)
        return updatePauseMenu(window, event);

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
    if (paused) return;
    player.update(dt, world);
    enemyManager.update(dt, world, player);   // ← добавить
    hud.update(player);
    updateCamera(window);
    // Проверка смерти
    if (player.isDeadByHP())
        deathScreenActive = true;
}

// ─── render ──────────────────────────────────────────────────────────────────

void PlayState::render(RenderWindow& window) {
    background.render(window, camera);
    world.render(window, camera);
    enemyManager.render(window);   // ← добавить (между миром и игроком)
    player.render(window);
    hud.render(window);
    if (paused) renderPauseMenu(window);
    if (deathScreenActive)
        renderDeathScreen(window);
}

void PlayState::renderDeathScreen(RenderWindow& window) {
    window.setView(window.getDefaultView());
    float W = (float)window.getSize().x;
    float H = (float)window.getSize().y;

    // Красный полупрозрачный оверлей
    RectangleShape overlay({ W, H });
    overlay.setFillColor(Color(160, 0, 0, 180));
    window.draw(overlay);

    auto makeText = [&](const wchar_t* str, int size, float y) {
        Text t;
        t.setFont(pauseFont);
        t.setString(str);
        t.setCharacterSize(size);
        t.setFillColor(Color::White);
        FloatRect r = t.getLocalBounds();
        t.setOrigin(r.left + r.width / 2.f, r.top + r.height / 2.f);
        t.setPosition(W / 2.f, y);
        window.draw(t);
        };

    makeText(L"Вы погибли", 80, H * 0.38f);
    makeText(L"-50 очков", 42, H * 0.50f);
    makeText(L"Нажмите любую клавишу для возрождения", 34, H * 0.60f);

    window.setView(camera);
}