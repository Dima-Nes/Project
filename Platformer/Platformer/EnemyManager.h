#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "Enemy.h"
#include "World.h"
#include "Player.h"
#include "Database.h"
#include "EnemyRecord.h"

using namespace sf;

class EnemyManager {
public:
    static constexpr float ACTIVE_RADIUS = 2500.f; // было 900

    void generate(const World& world, const std::string& username, Database* db);
    void savePositions(const std::string& username, Database* db);
    void update(float dt, const World& world, Player& player);
    void render(RenderWindow& window);

private:
    std::vector<std::unique_ptr<Enemy>> enemies;
    void spawnOne(EnemyType type, float worldX, float worldY, const World& world);
    static float dist(FloatRect a, FloatRect b);

    float spawnTimer = 0.f;
    static constexpr float SPAWN_INTERVAL = 4.f;    // секунд между спавнами
    static constexpr int   MAX_NEAR_ENEMIES = 12;     // макс врагов вокруг игрока
    static constexpr float SPAWN_MIN_DIST = 800.f;  // минимум — за экраном
    static constexpr float SPAWN_MAX_DIST = 1800.f; // максимум

    void dynamicSpawn(const World& world, Vector2f playerCenter);
};