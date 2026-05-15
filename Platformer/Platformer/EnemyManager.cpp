#include "EnemyManager.h"
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>

void EnemyManager::spawnOne(EnemyType type, float worldX, float worldY,
    const World& world)
{
    int tileX = (int)(worldX / World::TILE_SIZE);
    tileX = std::max(2, std::min(tileX, World::WIDTH - 3));
    float surfY = world.surfacePixelY(tileX);

    auto e = std::make_unique<Enemy>(type, worldX, surfY);

    const std::string path = (type == EnemyType::Orc)
        ? "assets/Orc.png" : "assets/Soldier.png";
    if (!e->loadTexture(path))
        std::cerr << "Enemy texture not found: " << path << "\n";

    enemies.push_back(std::move(e));
}

void EnemyManager::generate(const World& world,
    const std::string& username, Database* db)
{
    enemies.clear();

    std::vector<EnemyRecord> saved;
    if (db && db->loadEnemies(username, saved)) {
        for (auto& r : saved)
            spawnOne((EnemyType)r.type, r.x, r.y, world);
        std::cout << "Enemies loaded: " << enemies.size() << "\n";
        return;
    }

    std::srand(world.getSeed() ^ 0xDEADBEEF);
    const int SKIP = 60;
    const int ZONE = World::WIDTH - SKIP * 2;

    for (int i = 0; i < 150; ++i) {
        int tx = SKIP + std::rand() % ZONE;
        spawnOne(EnemyType::Orc, (float)(tx * World::TILE_SIZE), 0.f, world);
    }
    for (int i = 0; i < 100; ++i) {
        int tx = SKIP + std::rand() % ZONE;
        spawnOne(EnemyType::Soldier, (float)(tx * World::TILE_SIZE), 0.f, world);
    }

    // Враги гарантированно рядом со стартом
    const int CENTER = World::WIDTH / 2;
    std::srand(world.getSeed() ^ 0xAB1234);
    for (int i = 0; i < 8; ++i) {
        int tx = CENTER - 80 + std::rand() % 160;
        spawnOne(EnemyType::Orc, (float)(tx * World::TILE_SIZE), 0.f, world);
    }
    for (int i = 0; i < 5; ++i) {
        int tx = CENTER - 80 + std::rand() % 160;
        spawnOne(EnemyType::Soldier, (float)(tx * World::TILE_SIZE), 0.f, world);
    }

    std::srand((unsigned)std::time(nullptr));
    std::cout << "Enemies generated: " << enemies.size() << "\n";
}

void EnemyManager::savePositions(const std::string& username, Database* db) {
    if (!db) return;
    std::vector<EnemyRecord> data;
    for (auto& e : enemies) {
        Vector2f c = e->getCenter();
        EnemyRecord r;
        r.type = (int)e->getType();
        r.x = c.x;
        r.y = c.y;
        data.push_back(r);
    }
    db->saveEnemies(username, data);
    std::cout << "Enemies saved: " << data.size() << "\n";
}

void EnemyManager::update(float dt, const World& world, Player& player) {
    Vector2f playerCenter = player.getCenter();

    // ── Динамический спавн ───────────────────────────────────────────────────
    spawnTimer += dt;
    if (spawnTimer >= SPAWN_INTERVAL) {
        spawnTimer = 0.f;
        dynamicSpawn(world, playerCenter);
    }

    // ── Удаляем врагов слишком далеко от игрока ──────────────────────────────
    enemies.erase(
        std::remove_if(enemies.begin(), enemies.end(),
            [&](const std::unique_ptr<Enemy>& e) {
                if (e->isDead()) return true;
                float d = std::hypot(e->getCenter().x - playerCenter.x,
                    e->getCenter().y - playerCenter.y);
                return d > SPAWN_MAX_DIST * 1.5f;
            }),
        enemies.end());

    // ── Обновление и коллизии ────────────────────────────────────────────────
    FloatRect playerBox = player.getHitbox();

    for (auto& e : enemies) {
        Vector2f ec = e->getCenter();
        float d = std::hypot(ec.x - playerCenter.x, ec.y - playerCenter.y);
        if (d > ACTIVE_RADIUS) continue;
        if (!e->isAlive()) continue;

        e->update(dt, world, playerCenter);

        if (e->isHitting() && playerBox.intersects(e->getHitbox())) {
            player.loseLife();
            e->resetHit();
        }

        if (player.getIsAttacking() && playerBox.intersects(e->getHitbox())) {
            e->takeDamage(1);
            if (!e->isAlive())
                player.addScore(10);
        }
    }
}

void EnemyManager::render(RenderWindow& window) {
    for (auto& e : enemies)
        e->render(window);
}

float EnemyManager::dist(FloatRect a, FloatRect b) {
    return std::hypot(a.left + a.width / 2.f - b.left - b.width / 2.f,
        a.top + a.height / 2.f - b.top - b.height / 2.f);
}

void EnemyManager::dynamicSpawn(const World& world, Vector2f playerCenter) {
    // Считаем врагов рядом
    int nearCount = 0;
    for (auto& e : enemies) {
        float d = std::hypot(e->getCenter().x - playerCenter.x,
            e->getCenter().y - playerCenter.y);
        if (d < SPAWN_MAX_DIST) nearCount++;
    }
    if (nearCount >= MAX_NEAR_ENEMIES) return;

    // Спавним справа или слева от игрока за пределами видимости
    int side = (std::rand() % 2 == 0) ? 1 : -1;
    float dist = SPAWN_MIN_DIST + std::rand() % (int)(SPAWN_MAX_DIST - SPAWN_MIN_DIST);
    float spawnX = playerCenter.x + side * dist;

    int tileX = (int)(spawnX / World::TILE_SIZE);
    tileX = std::max(2, std::min(tileX, World::WIDTH - 3));

    EnemyType type = (std::rand() % 2 == 0) ? EnemyType::Orc : EnemyType::Soldier;
    spawnOne(type, (float)(tileX * World::TILE_SIZE), 0.f, world);
}