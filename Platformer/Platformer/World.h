#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdint>

using namespace sf;

// ─── Типы тайлов ──────────────────────────────────────────────────────────────
enum class TileType : uint8_t {
    Air = 0,
    Grass = 1,
    Dirt = 2,
    Stone = 3
};

// ─── World ────────────────────────────────────────────────────────────────────
// Хранит тайловую карту, генерирует её по сиду (шум Перлина),
// рисует только видимую область через VertexArray (быстро).

class World {
public:
    // ─── Параметры мира (меняй здесь) ─────────────────────────────────────────
    static constexpr int TILE_SIZE = 32;   // пикселей на тайл
    static constexpr int WIDTH = 800;  // тайлов в ширину
    static constexpr int HEIGHT = 150;  // тайлов в высоту
    static constexpr int SEA_LEVEL = 80;   // базовая строка поверхности

private:
    std::vector<std::vector<TileType>> tiles; // tiles[x][y]
    int seed = 0;

    // ─── Шум ──────────────────────────────────────────────────────────────────
    // valueNoise: псевдослучайное значение [0,1] для целого x
    float valueNoise(int x) const;
    // smoothNoise: интерполирует между двумя valueNoise с smoothstep
    float smoothNoise(float x) const;
    // octaveNoise: суммирует несколько октав (fractal noise)
    float octaveNoise(float x, int octaves, float persistence) const;

public:
    World();

    // Генерировать мир по сиду. Вызывать один раз перед игрой.
    void generate(int s);

    // ─── Доступ к тайлам ──────────────────────────────────────────────────────
    TileType get(int x, int y) const;
    bool     isSolid(int x, int y) const;

    // Пиксельные координаты → твёрдый ли тайл?
    bool     isSolidAt(float px, float py) const;

    // Pixel Y верхней грани поверхности в колонке tileX
    float    surfacePixelY(int tileX) const;

    int      getSeed() const { return seed; }

    // ─── Рендер ───────────────────────────────────────────────────────────────
    // Рисует только тайлы, попадающие в текущий View камеры.
    void render(RenderWindow& window, const View& cam) const;
};