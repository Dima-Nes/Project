#include "World.h"
#include <cmath>
#include <algorithm>
#include <numeric>
#include <random>

World::World() {
    tiles.assign(WIDTH, std::vector<TileType>(HEIGHT, TileType::Air));
}

// ─── Шум ──────────────────────────────────────────────────────────────────────

float World::valueNoise(int x) const {
    // Быстрый детерминированный хеш → [0, 1]
    int n = x + seed * 57;
    n = (n << 13) ^ n;
    n = n * (n * n * 15731 + 789221) + 1376312589;
    return (float)(n & 0x7FFFFFFF) / (float)0x7FFFFFFF;
}

float World::smoothNoise(float x) const {
    int   ix = (int)std::floor(x);
    float fx = x - (float)ix;
    float t = fx * fx * (3.f - 2.f * fx);  // smoothstep
    return valueNoise(ix) * (1.f - t) + valueNoise(ix + 1) * t;
}

float World::octaveNoise(float x, int octaves, float persistence) const {
    float value = 0.f, amplitude = 1.f, frequency = 1.f, maxValue = 0.f;
    for (int i = 0; i < octaves; ++i) {
        value += smoothNoise(x * frequency) * amplitude;
        maxValue += amplitude;
        amplitude *= persistence;
        frequency *= 2.f;
    }
    return value / maxValue;  // нормализуем к [0, 1]
}

// ─── Генерация ────────────────────────────────────────────────────────────────

void World::generate(int s) {
    seed = s;

    for (int x = 0; x < WIDTH; ++x) {
        // Высота поверхности: плавный рельеф ±25 тайлов от SEA_LEVEL
        float n = octaveNoise(x * 0.007f, 5, 0.55f);          // [0, 1]
        int   surf = SEA_LEVEL + (int)((n - 0.5f) * 50.f);
        surf = std::max(15, std::min(surf, HEIGHT - 20));

        for (int y = 0; y < HEIGHT; ++y) {
            if (y < surf)      tiles[x][y] = TileType::Air;
            else if (y == surf)     tiles[x][y] = TileType::Grass;
            else if (y < surf + 6)  tiles[x][y] = TileType::Dirt;
            else                    tiles[x][y] = TileType::Stone;
        }
    }
}

// ─── Доступ ───────────────────────────────────────────────────────────────────

TileType World::get(int x, int y) const {
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) return TileType::Stone;
    return tiles[x][y];
}

bool World::isSolid(int x, int y) const {
    return get(x, y) != TileType::Air;
}

bool World::isSolidAt(float px, float py) const {
    return isSolid((int)std::floor(px / TILE_SIZE),
        (int)std::floor(py / TILE_SIZE));
}

float World::surfacePixelY(int tileX) const {
    tileX = std::max(0, std::min(tileX, WIDTH - 1));
    for (int y = 0; y < HEIGHT; ++y) {
        if (tiles[tileX][y] != TileType::Air)
            return (float)(y * TILE_SIZE);
    }
    return (float)((HEIGHT - 1) * TILE_SIZE);
}

// ─── Рендер ───────────────────────────────────────────────────────────────────
// Используем VertexArray — один вызов draw() на весь видимый мир.

void World::render(RenderWindow& window, const View& cam) const {
    Vector2f center = cam.getCenter();
    Vector2f half = cam.getSize() * 0.5f;

    int x0 = std::max(0, (int)std::floor((center.x - half.x) / TILE_SIZE) - 1);
    int x1 = std::min(WIDTH - 1, (int)std::floor((center.x + half.x) / TILE_SIZE) + 1);
    int y0 = std::max(0, (int)std::floor((center.y - half.y) / TILE_SIZE) - 1);
    int y1 = std::min(HEIGHT - 1, (int)std::floor((center.y + half.y) / TILE_SIZE) + 1);

    VertexArray va(Quads);
    va.resize((x1 - x0 + 1) * (y1 - y0 + 1) * 4);
    int idx = 0;

    for (int x = x0; x <= x1; ++x) {
        for (int y = y0; y <= y1; ++y) {
            TileType t = tiles[x][y];
            if (t == TileType::Air) { idx += 4; continue; }

            Color c;
            switch (t) {
            case TileType::Grass: c = Color(72, 160, 28); break;
            case TileType::Dirt:  c = Color(140, 90, 40); break;
            case TileType::Stone: c = Color(108, 108, 108); break;
            default: idx += 4; continue;
            }

            float px = (float)(x * TILE_SIZE);
            float py = (float)(y * TILE_SIZE);
            float ts = (float)TILE_SIZE;

            // Слегка затемняем боковые грани для объёма
            Color side = Color(c.r * 7 / 8, c.g * 7 / 8, c.b * 7 / 8);

            va[idx + 0] = Vertex({ px,      py }, (t == TileType::Grass ? Color(90, 190, 40) : c));
            va[idx + 1] = Vertex({ px + ts, py }, (t == TileType::Grass ? Color(90, 190, 40) : c));
            va[idx + 2] = Vertex({ px + ts, py + ts }, side);
            va[idx + 3] = Vertex({ px,      py + ts }, side);
            idx += 4;
        }
    }

    window.draw(va);
}