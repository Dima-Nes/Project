#include "World.h"
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <ctime>

World::World() {
    tiles.assign(WIDTH, std::vector<TileType>(HEIGHT, TileType::Air));
    decors.assign(WIDTH, DecorType::None);
}

// ─── Текстуры ─────────────────────────────────────────────────────────────────

bool World::loadTextures(const std::string& path) {
    const char* names[4] = {
        "spring_.png", "autumn_.png", "summer_.png", "winter_.png"
    };
    bool ok = true;
    for (int i = 0; i < 4; ++i) {
        texLoaded[i] = biomeTex[i].loadFromFile(path + names[i]);
        if (!texLoaded[i]) ok = false;
    }
    decorTexLoaded = decorTex.loadFromFile(path + "staticObjects_.png");
    return ok;
}

// ─── Шум ──────────────────────────────────────────────────────────────────────

float World::valueNoise(int x) const {
    int n = x + seed * 57;
    n = (n << 13) ^ n;
    n = n * (n * n * 15731 + 789221) + 1376312589;
    return (float)(n & 0x7FFFFFFF) / (float)0x7FFFFFFF;
}
float World::smoothNoise(float x) const {
    int   ix = (int)std::floor(x);
    float fx = x - ix;
    float t = fx * fx * (3.f - 2.f * fx);
    return valueNoise(ix) * (1.f - t) + valueNoise(ix + 1) * t;
}
float World::octaveNoise(float x, int octaves, float persistence) const {
    float v = 0, a = 1, f = 1, m = 0;
    for (int i = 0;i < octaves;++i) { v += smoothNoise(x * f) * a;m += a;a *= persistence;f *= 2; }
    return v / m;
}

// ─── Биомы ────────────────────────────────────────────────────────────────────

void World::generateBiomes() {
    biomes.clear();
    std::srand(seed ^ 0xB10DE5);
    const int spawnX = WIDTH / 2;
    int forestL = spawnX - 75, forestR = spawnX + 75;
    biomes.push_back({ forestL, forestR, BiomeType::Forest });

    // ── Биомы вправо ─────────────────────────────────────────────────────────
    BiomeType rBiomes[] = { BiomeType::Autumn, BiomeType::Desert, BiomeType::Winter };
    int cur = forestR, idx = 0;
    while (cur < WIDTH - 2) {
        int end = std::min(cur + 200 + std::rand() % 201, WIDTH - 2);
        biomes.push_back({ cur, end, rBiomes[idx % 3] });
        cur = end; ++idx;
    }

    // ── Биомы влево (Зима всегда первая) ─────────────────────────────────────
    BiomeType lBiomes[] = { BiomeType::Winter, BiomeType::Desert, BiomeType::Autumn };
    cur = forestL; idx = 0;
    while (cur > 2) {
        int start = std::max(cur - 200 - std::rand() % 201, 2);
        biomes.insert(biomes.begin(), { start, cur, lBiomes[idx % 3] });
        cur = start; ++idx;
    }

    std::srand((unsigned)std::time(nullptr));
}

BiomeType World::getBiomeAt(int tileX) const {
    for (auto& z : biomes)
        if (tileX >= z.startX && tileX < z.endX) return z.type;
    return BiomeType::Forest;
}

// ─── Рельеф ───────────────────────────────────────────────────────────────────

struct BiomeParams { float amp, freq, base; int dirtD; };
static BiomeParams getBiomeParams(BiomeType b) {
    // Одинаковая freq у всех — убирает стены на границах!
    switch (b) {
    case BiomeType::Forest: return { 40.f, 0.007f, 80.f, 5 };
    case BiomeType::Autumn: return { 50.f, 0.007f, 79.f, 4 };
    case BiomeType::Desert: return { 15.f, 0.007f, 83.f, 3 };
    case BiomeType::Winter: return { 60.f, 0.007f, 77.f, 6 };
    default:                return { 40.f, 0.007f, 80.f, 5 };
    }
}

void World::generateTerrain() {
    // ── Проход 1: вычислить высоту для каждого x ──────────────────────────────
    std::vector<float> H(WIDTH);
    for (int x = 0; x < WIDTH; ++x) {
        BiomeParams p = getBiomeParams(getBiomeAt(x));
        float n = octaveNoise(x * p.freq, 5, 0.55f);
        H[x] = p.base + (n - 0.5f) * p.amp;
    }

    // ── Проход 2: сгладить высоты на границах биомов ──────────────────────────
    const int TRANS = 80;
    for (int b = 1; b < (int)biomes.size(); ++b) {
        int bx = biomes[b].startX; // граница между biomes[b-1] и biomes[b]

        BiomeParams pL = getBiomeParams(biomes[b - 1].type);
        BiomeParams pR = getBiomeParams(biomes[b].type);

        for (int dx = -TRANS; dx <= TRANS; ++dx) {
            int x = bx + dx;
            if (x < 2 || x >= WIDTH - 2) continue;

            // Высота как если бы биом был левым
            float nL = octaveNoise(x * pL.freq, 5, 0.55f);
            float hL = pL.base + (nL - 0.5f) * pL.amp;

            // Высота как если бы биом был правым
            float nR = octaveNoise(x * pR.freq, 5, 0.55f);
            float hR = pR.base + (nR - 0.5f) * pR.amp;

            // smoothstep от 0 (левый) до 1 (правый)
            float t = (float)(dx + TRANS) / (float)(2 * TRANS);
            t = t * t * (3.f - 2.f * t);

            H[x] = hL * (1.f - t) + hR * t;
        }
    }

    // ── Проход 3: заполнить тайлы ────────────────────────────────────────────
    for (int x = 0; x < WIDTH; ++x) {
        int surf = (int)std::max(15.f, std::min(H[x], (float)(HEIGHT - 20)));
        int dirtD = getBiomeParams(getBiomeAt(x)).dirtD;

        for (int y = 0; y < HEIGHT; ++y) {
            if (y < surf)          tiles[x][y] = TileType::Air;
            else if (y == surf)         tiles[x][y] = TileType::Grass;
            else if (y < surf + dirtD)  tiles[x][y] = TileType::Dirt;
            else                        tiles[x][y] = TileType::Stone;
        }
    }

    // Каменные стены по краям
    for (int y = 0; y < HEIGHT; ++y) {
        tiles[0][y] = tiles[1][y] = TileType::Stone;
        tiles[WIDTH - 1][y] = tiles[WIDTH - 2][y] = TileType::Stone;
    }
}

// ─── Декорации ────────────────────────────────────────────────────────────────

void World::generateDecorations() {
    decors.assign(WIDTH, DecorType::None);
    std::srand(seed ^ 0xDEC0);

    for (int x = 2; x < WIDTH - 2; ++x) {
        // Убеждаемся что есть поверхность
        bool hasSurf = false;
        for (int y = 0;y < HEIGHT;++y) if (tiles[x][y] != TileType::Air) { hasSurf = true;break; }
        if (!hasSurf) continue;

        // Не ставим декор вплотную к другому дереву (пропускаем 2 тайла)
        if (x > 2 && (decors[x - 1] == DecorType::Tree || decors[x - 1] == DecorType::Palm
            || decors[x - 1] == DecorType::SmallTree)) continue;
        if (x > 3 && (decors[x - 2] == DecorType::Tree || decors[x - 2] == DecorType::Palm
            || decors[x - 2] == DecorType::SmallTree)) continue;

        int r = std::rand() % 100;
        BiomeType b = getBiomeAt(x);

        switch (b) {
        case BiomeType::Forest:
            if (r < 15) decors[x] = DecorType::Tree;
            else if (r < 22) decors[x] = DecorType::Bush;
            else if (r < 29) decors[x] = DecorType::Pumpkin;
            break;
        case BiomeType::Autumn:
            if (r < 12) decors[x] = DecorType::Tree;
            else if (r < 20) decors[x] = DecorType::SmallTree;
            break;
        case BiomeType::Desert:
            if (r < 10) decors[x] = DecorType::Palm;
            break;
        case BiomeType::Winter:
            if (r < 18) decors[x] = DecorType::SmallTree;
            break;
        }
    }
    std::srand((unsigned)std::time(nullptr));
}

void World::generate(int s) {
    seed = s;
    generateBiomes();
    generateTerrain();
    generateDecorations();
}

// ─── Доступ ───────────────────────────────────────────────────────────────────

TileType World::get(int x, int y) const {
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) return TileType::Stone;
    return tiles[x][y];
}
bool World::isSolid(int x, int y) const { return get(x, y) != TileType::Air; }
bool World::isSolidAt(float px, float py) const {
    return isSolid((int)std::floor(px / TILE_SIZE), (int)std::floor(py / TILE_SIZE));
}
float World::surfacePixelY(int tileX) const {
    tileX = std::max(0, std::min(tileX, WIDTH - 1));
    for (int y = 0;y < HEIGHT;++y) if (tiles[tileX][y] != TileType::Air) return (float)(y * TILE_SIZE);
    return (float)((HEIGHT - 1) * TILE_SIZE);
}

int World::getTextureBiome(int x) const {
    const int TRANS = 50;
    BiomeType cur = getBiomeAt(x);
    int bi = (int)cur;
    for (auto& zone : biomes) {
        if (zone.type != cur) continue;
        int distR = zone.endX - x;
        if (distR >= 0 && distR < TRANS) {
            BiomeType next = cur;
            for (auto& z2 : biomes) if (z2.startX == zone.endX) { next = z2.type;break; }
            float t = 1.f - (float)distR / TRANS;
            int hash = (x * 2341 + zone.endX * 137) & 0x7FFF;
            if ((float)hash / 0x7FFF < t) bi = (int)next;
        }
        int distL = x - zone.startX;
        if (distL >= 0 && distL < TRANS) {
            BiomeType prev = cur;
            for (auto& z2 : biomes) if (z2.endX == zone.startX) { prev = z2.type;break; }
            float t = 1.f - (float)distL / TRANS;
            int hash = (x * 2341 + zone.startX * 137) & 0x7FFF;
            if ((float)hash / 0x7FFF < t) bi = (int)prev;
        }
    }
    return bi;
}

// ─── Рендер тайлов ────────────────────────────────────────────────────────────
//
// UV тайлов в биом-листах (одинаковы для всех 4 листов):
//   Grass : (2, 50, 12, 12)
//   Dirt  : (2, 62, 12, 12)
//   Stone : цветной фолбэк (добавим позже)

void World::render(RenderWindow& window, const View& cam) const {
    Vector2f center = cam.getCenter();
    Vector2f half = cam.getSize() * 0.5f;

    int x0 = std::max(0, (int)std::floor((center.x - half.x) / TILE_SIZE) - 1);
    int x1 = std::min(WIDTH - 1, (int)std::floor((center.x + half.x) / TILE_SIZE) + 1);
    int y0 = std::max(0, (int)std::floor((center.y - half.y) / TILE_SIZE) - 1);
    int y1 = std::min(HEIGHT - 1, (int)std::floor((center.y + half.y) / TILE_SIZE) + 1);

    // Предвычисляем Y поверхности для каждой видимой колонки
    std::vector<int> surfTile(x1 - x0 + 1, SEA_LEVEL);
    for (int x = x0; x <= x1; ++x) {
        for (int y = 0; y < HEIGHT; ++y) {
            if (tiles[x][y] != TileType::Air) { surfTile[x - x0] = y; break; }
        }
    }


    VertexArray biomeVA[4];
    for (int i = 0;i < 4;++i) biomeVA[i].setPrimitiveType(Quads);

    // Stone фолбэки по биому
    const Color stoneFB[4] = {
        Color(108,108,108),Color(100,95,90),Color(150,130,100),Color(130,140,160)
    };

    for (int x = x0; x <= x1; ++x) {
        int bi = getTextureBiome(x);
        int surf = surfTile[x - x0];  // поверхность этой колонки

        for (int y = y0; y <= y1; ++y) {
            TileType t = tiles[x][y];
            if (t == TileType::Air) continue;

            float px = (float)(x * TILE_SIZE);
            float py = (float)(y * TILE_SIZE);
            float ts = (float)TILE_SIZE;

            // ── Яркость по глубине ───────────────────────────────────────────
            int depth = y - surf;
            float brightness = (depth <= 0)
                ? 1.0f
                : std::max(0.05f, 1.0f - (float)depth / 15.0f);
            sf::Uint8 br = (sf::Uint8)(brightness * 255.f);
            Color tint(br, br, br);

            if (texLoaded[bi]) {
                float u0, v0;
                if (t == TileType::Grass) { u0 = 2.f;   v0 = 34.f; }
                else if (t == TileType::Dirt) { u0 = 102.f; v0 = 66.f; }
                else { u0 = 100.f; v0 = 114.f; }
                float u1 = u0 + 12.f, v1 = v0 + 12.f;

                biomeVA[bi].append({ {px,    py   }, tint, {u0, v0} });
                biomeVA[bi].append({ {px + ts, py   }, tint, {u1, v0} });
                biomeVA[bi].append({ {px + ts, py + ts}, tint, {u1, v1} });
                biomeVA[bi].append({ {px,    py + ts}, tint, {u0, v1} });
            }
            else {
                const Color grassFB[4] = { Color(72,160,28),Color(180,130,30),Color(180,160,50),Color(200,220,210) };
                const Color dirtFB[4] = { Color(140,90,40),Color(130,80,30),Color(180,140,60),Color(140,110,90) };
                Color base = (t == TileType::Grass) ? grassFB[bi] : (t == TileType::Dirt) ? dirtFB[bi] : stoneFB[bi];

                // Применяем затемнение к фолбэк-цвету
                Color c(base.r * br / 255, base.g * br / 255, base.b * br / 255);

                biomeVA[bi].append({ {px,    py   }, c });
                biomeVA[bi].append({ {px + ts, py   }, c });
                biomeVA[bi].append({ {px + ts, py + ts}, c });
                biomeVA[bi].append({ {px,    py + ts}, c });
            }
        }
    }

    for (int i = 0;i < 4;++i) {
        if (biomeVA[i].getVertexCount() == 0) continue;
        if (texLoaded[i]) window.draw(biomeVA[i], &biomeTex[i]);
        else              window.draw(biomeVA[i]);
    }

    renderDecorations(window, cam);
}

// ─── Рендер декораций ────────────────────────────────────────────────────────
//
// Координаты спрайтов в staticObjects_.png (288×144):
//   Пальма      (34×45) : x=55,  y=14
//   Ель малая   (16×27) : x=192, y=37
//   Ель большая (23×41) : x=156, y=23
//   Тыква       (14×14) : x=193, y=18
//   Куст        (14×14) : x=33,  y=18
//
// Трава, высокая трава, мёртвое дерево, кактус — цветной фолбэк.

struct DecorUV {
    int tx, ty, tw, th;    // UV в текстуре
    int dw, dh;          // размер на экране (пиксели)
};

static DecorUV getDecorUV(DecorType d) {
    // Масштаб ~3x для пиксель-арт стиля
    switch (d) {
    case DecorType::Tree:      return { 156,23,23,41, 23 * 3,41 * 3 }; // большая ель
    case DecorType::SmallTree: return { 192,37,16,27, 16 * 3,27 * 3 }; // маленькая ель
    case DecorType::Palm:      return { 55,14,34,45, 34 * 3,45 * 3 }; // пальма
    case DecorType::Pumpkin:   return { 193,18,14,14, 14 * 3,14 * 3 }; // тыква
    case DecorType::Bush:      return { 33,18,14,14, 14 * 3,14 * 3 }; // куст
    default: return { 0,0,0,0,0,0 };
    }
}

void World::renderDecorations(RenderWindow& window, const View& cam) const {
    Vector2f center = cam.getCenter();
    Vector2f half = cam.getSize() * 0.5f;
    int x0 = std::max(2, (int)std::floor((center.x - half.x) / TILE_SIZE) - 1);
    int x1 = std::min(WIDTH - 3, (int)std::floor((center.x + half.x) / TILE_SIZE) + 1);

    // Описание спрайта: (srcX, srcY, srcW, srcH, renderScale)
    struct Spr { float x, y, w, h, s; };
    // Было: { 55, 14, 34, 45, 1.6f }
    const Spr PALM = { 55, 14, 34, 53, 2.4f }; // h=53 (+8), scale увеличен
    const Spr TREE_L = { 156, 23, 23, 41, 1.5f };  // было 2.0
    const Spr TREE_S = { 192, 37, 16, 27, 1.4f };  // было 2.0
    const Spr PUMPKIN = { 193, 18, 14, 14, 1.2f };  // было 1.5
    const Spr BUSH_S = { 33,  18, 14, 14, 1.8f }; // было 2.5f

    VertexArray vaSprite(Quads); // со спрайтами из decorTex
    VertexArray vaColor(Quads); // трава и прочее цветом

    auto addSprite = [&](float cx, float surfY, const Spr& sp) {
        float ww = sp.w * sp.s;
        float wh = sp.h * sp.s;
        float wx = cx - ww / 2.f;
        float wy = surfY - wh;
        vaSprite.append({ {wx,    wy   }, Color::White, {sp.x,      sp.y     } });
        vaSprite.append({ {wx + ww, wy   }, Color::White, {sp.x + sp.w, sp.y     } });
        vaSprite.append({ {wx + ww, wy + wh}, Color::White, {sp.x + sp.w, sp.y + sp.h} });
        vaSprite.append({ {wx,    wy + wh}, Color::White, {sp.x,      sp.y + sp.h} });
        };

    auto addRect = [&](float rx, float ry, float rw, float rh, Color c) {
        vaColor.append({ {rx,    ry   }, c });
        vaColor.append({ {rx + rw, ry   }, c });
        vaColor.append({ {rx + rw, ry + rh}, c });
        vaColor.append({ {rx,    ry + rh}, c });
        };

    for (int x = x0; x <= x1; ++x) {
        DecorType d = decors[x];
        if (d == DecorType::None) continue;

        int surf = -1;
        for (int y = 0; y < HEIGHT; ++y)
            if (tiles[x][y] != TileType::Air) { surf = y; break; }
        if (surf < 0) continue;

        float cx = (float)(x * TILE_SIZE) + TILE_SIZE * 0.5f;
        float spy = (float)(surf * TILE_SIZE);
        float ts = (float)TILE_SIZE;
        int   bi = (int)getBiomeAt(x);

        Color grassC = (bi == 3) ? Color(200, 220, 240) : (bi == 2) ? Color(180, 160, 60) : Color(80, 180, 50);

        switch (d) {
        case DecorType::Tree:      addSprite(cx, spy, TREE_L);   break;
        case DecorType::SmallTree: addSprite(cx, spy, TREE_S);   break;
        case DecorType::Palm:      addSprite(cx, spy, PALM);     break;
        case DecorType::Pumpkin:   addSprite(cx, spy, PUMPKIN);  break;
        case DecorType::Bush:      addSprite(cx, spy, BUSH_S);   break;
        case DecorType::DeadTree:
            addRect(cx - ts * 0.1f, spy - ts * 2.f, ts * 0.2f, ts * 2.f, Color(101, 67, 33));
            addRect(cx - ts * 0.4f, spy - ts * 1.8f, ts * 0.3f, ts * 0.1f, Color(101, 67, 33));
            addRect(cx + ts * 0.1f, spy - ts * 1.5f, ts * 0.3f, ts * 0.1f, Color(101, 67, 33));
            break;
        case DecorType::Cactus:
            addRect(cx - ts * 0.15f, spy - ts * 1.8f, ts * 0.3f, ts * 1.8f, Color(80, 140, 40));
            addRect(cx - ts * 0.45f, spy - ts * 1.3f, ts * 0.3f, ts * 0.12f, Color(80, 140, 40));
            addRect(cx + ts * 0.15f, spy - ts * 1.1f, ts * 0.3f, ts * 0.12f, Color(80, 140, 40));
            break;
        case DecorType::TallGrass:
            addRect(cx - ts * 0.15f, spy - ts * 0.5f, ts * 0.12f, ts * 0.5f, grassC);
            addRect(cx + ts * 0.03f, spy - ts * 0.4f, ts * 0.12f, ts * 0.4f, grassC);
            break;
        case DecorType::Grass:
            addRect(cx - ts * 0.2f, spy - ts * 0.25f, ts * 0.4f, ts * 0.25f, grassC);
            break;
        default: break;
        }
    }

    if (decorTexLoaded && vaSprite.getVertexCount() > 0)
        window.draw(vaSprite, &decorTex);
    if (vaColor.getVertexCount() > 0)
        window.draw(vaColor);
}