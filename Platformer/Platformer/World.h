#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdint>

using namespace sf;

enum class TileType : uint8_t {
    Air = 0, Grass = 1, Dirt = 2, Stone = 3
};

enum class BiomeType : uint8_t {
    Forest = 0,
    Autumn = 1,
    Desert = 2,
    Winter = 3
};

enum class DecorType : uint8_t {
    None = 0,
    Grass,          // короткая трава
    TallGrass,      // высокая трава
    Tree,           // большая ель (лес/осень)
    SmallTree,      // маленькая ель (зима)
    Palm,           // пальма (пустыня)
    DeadTree,       // мёртвое дерево
    Cactus,         // кактус
    Bush,           // куст
    Pumpkin         // тыква (лес)
};

struct BiomeZone {
    int startX, endX;
    BiomeType type;
};

class World {
public:
    static constexpr int TILE_SIZE = 32;
    static constexpr int WIDTH = 8000;  // было 800
    static constexpr int HEIGHT = 200;   // было 150, глубже для большего мира
    static constexpr int SEA_LEVEL = 110;   // было 80, пропорционально

    // ─── UV тайлов в листах биомов (одинаковы для всех 4 листов) ─────────────
    // Grass: (2, 50, 12, 12)   Dirt: (2, 62, 12, 12)
    // Stone: цвет-фолбэк (добавим позже)

    // ─── UV декораций в staticObjects_.png (288×144) ──────────────────────────
    static constexpr int DEC_SCALE = 3; // пиксель-арт масштаб декораций

private:
    int getTextureBiome(int x) const;

    std::vector<std::vector<TileType>> tiles;
    std::vector<DecorType>             decors;
    std::vector<BiomeZone>             biomes;

    Texture biomeTex[4];
    bool    texLoaded[4] = {};

    Texture decorTex;
    bool    decorTexLoaded = false;

    int seed = 0;

    float valueNoise(int x) const;
    float smoothNoise(float x) const;
    float octaveNoise(float x, int octaves, float persistence) const;

    void generateBiomes();
    void generateTerrain();
    void generateDecorations();

    void renderDecorations(RenderWindow& window, const View& cam) const;

public:
    World();
    bool loadTextures(const std::string& path = "assets/");
    void generate(int s);


    TileType  get(int x, int y)           const;
    bool      isSolid(int x, int y)       const;
    bool      isSolidAt(float px, float py) const;
    float     surfacePixelY(int tileX)    const;
    BiomeType getBiomeAt(int tileX)       const;
    int       getSeed()                   const { return seed; }

    void render(RenderWindow& window, const View& cam) const;
};