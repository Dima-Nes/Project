#include "Background.h"
#include <cmath>
#include <cstdlib>
#include <iostream>

static const float WORLD_MID_X = (float)(World::WIDTH * World::TILE_SIZE) / 2.f;
static const float SURFACE_Y = (float)(World::SEA_LEVEL * World::TILE_SIZE);

bool Background::load(const std::string& folder, const std::string& cloudFolder) {

    if (skyTex.loadFromFile(folder + "/forest/forest_sky.png")) {
        skyTex.setSmooth(false);
        skyTex.setRepeated(true);
        hasSky = true;
    }
    else std::cerr << "BG: forest_sky.png not found\n";

    // parallaxX: насколько медленнее камеры движется по горизонтали
    // parallaxY: насколько медленнее камеры движется по вертикали
    //
    // ВАЖНО: parallaxY должен быть БЛИЗОК к parallaxX.
    // Если parallaxY мал — слой "падает" вниз при взгляде вверх и закрывает небо.
    // Если parallaxY велик — слой движется почти как камера, глубины нет.
    struct Desc { std::string path; float px; float py; };
    std::vector<Desc> descs = {
        { folder + "/forest/forest_mountain.png", 0.10f, 0.10f },
        { folder + "/forest/forest_back.png",     0.20f, 0.20f },
        { folder + "/forest/forest_mid.png",      0.32f, 0.32f },
        { folder + "/forest/forest_long.png",     0.44f, 0.44f },
        { folder + "/forest/forest_short.png",    0.60f, 0.60f },
    };

    for (auto& d : descs) {
        BgLayer layer;
        if (!layer.texture.loadFromFile(d.path)) {
            std::cerr << "BG: not found: " << d.path << "\n";
            continue;
        }
        layer.texture.setSmooth(false);
        layer.texture.setRepeated(true);
        layer.parallaxX = d.px;
        layer.parallaxY = d.py;
        layers.push_back(std::move(layer));
    }

    for (int i = 1; i <= 20; ++i) {
        Texture t;
        if (t.loadFromFile(cloudFolder + "/Cloud " + std::to_string(i) + ".png")) {
            t.setSmooth(false);
            cloudTextures.push_back(std::move(t));
        }
    }
    if (cloudTextures.empty())
        std::cerr << "BG: no clouds in " << cloudFolder << "\n";

    return true;
}

// ─── Облака ──────────────────────────────────────────────────────────────────

void Background::initClouds(const View& camera) {
    if (cloudTextures.empty()) return;
    cloudsInited = true;
    clouds.clear();

    float camX = camera.getCenter().x;
    float camW = camera.getSize().x;
    float camH = camera.getSize().y;

    float cloudMinY = SURFACE_Y - camH * 1.2f;
    float cloudMaxY = SURFACE_Y - camH * 0.15f;

    for (int i = 0; i < CLOUD_COUNT; ++i) {
        CloudSprite c;
        int idx = std::rand() % (int)cloudTextures.size();
        c.scale = 0.7f + (std::rand() % 70) / 100.f;
        c.speed = 12.f + std::rand() % 40;
        c.x = camX - camW * 0.5f + (float)(std::rand() % (int)(camW * 1.5f));
        c.y = cloudMinY + (float)(std::rand() % std::max(1, (int)(cloudMaxY - cloudMinY)));
        c.sprite.setTexture(cloudTextures[idx]);
        c.sprite.setScale(c.scale, c.scale);
        c.sprite.setPosition(c.x, c.y);
        clouds.push_back(std::move(c));
    }
}

void Background::wrapCloud(CloudSprite& c, const View& camera) {
    float camX = camera.getCenter().x;
    float camW = camera.getSize().x;
    float camH = camera.getSize().y;
    auto* tex = c.sprite.getTexture();
    if (!tex) return;
    float cw = tex->getSize().x * c.scale;

    if (c.x > camX + camW * 0.5f + cw) {
        int idx = std::rand() % (int)cloudTextures.size();
        c.sprite.setTexture(cloudTextures[idx]);
        c.scale = 0.7f + (std::rand() % 70) / 100.f;
        c.speed = 12.f + std::rand() % 40;
        c.x = camX - camW * 0.5f - cloudTextures[idx].getSize().x * c.scale;

        float cloudMinY = SURFACE_Y - camH * 1.2f;
        float cloudMaxY = SURFACE_Y - camH * 0.15f;
        c.y = cloudMinY + (float)(std::rand() % std::max(1, (int)(cloudMaxY - cloudMinY)));
        c.sprite.setScale(c.scale, c.scale);
    }
}

void Background::update(float dt, const View& camera) {
    if (!cloudsInited) initClouds(camera);
    for (auto& c : clouds) {
        c.x += c.speed * dt;
        c.sprite.setPosition(c.x, c.y);
        wrapCloud(c, camera);
    }
}

// ─── Небо ─────────────────────────────────────────────────────────────────────

void Background::drawSky(RenderWindow& window, const View& camera) {
    if (!hasSky) return;
    Vector2f cc = camera.getCenter();
    Vector2f cs = camera.getSize();
    Vector2u ts = skyTex.getSize();

    // Небо от поверхности вверх на 3 высоты экрана
    float skyBottom = SURFACE_Y;
    float skyHeight = cs.y * 3.f;
    float skyTop = skyBottom - skyHeight;
    float scaleY = skyHeight / (float)ts.y;

    float offsetX = std::fmod((cc.x - WORLD_MID_X) * 0.04f, (float)ts.x);
    if (offsetX < 0) offsetX += ts.x;

    float startX = cc.x - cs.x * 0.5f - (float)ts.x;
    int   copies = (int)(cs.x / ts.x) + 3;

    Sprite spr(skyTex);
    spr.setScale(1.f, scaleY);
    for (int i = 0; i < copies; ++i) {
        spr.setPosition(startX + i * (float)ts.x - offsetX, skyTop);
        window.draw(spr);
    }
}

// ─── Фоновый слой ────────────────────────────────────────────────────────────
//
// Ключевая формула Y-параллакса — lerp между позицией камеры и поверхностью:
//
//   renderBottomY = SURFACE_Y + (cc.y - SURFACE_Y) * parallaxY
//
// При parallaxY = 0: слой всегда у SURFACE_Y (неподвижен по Y)
// При parallaxY = 1: слой движется вместе с камерой
// При parallaxX == parallaxY: нет эффекта "разъезжания" — глубина без тошноты

void Background::drawLayer(RenderWindow& window, const View& camera,
    const BgLayer& layer)
{
    Vector2f cc = camera.getCenter();
    Vector2f cs = camera.getSize();
    Vector2u ts = layer.texture.getSize();

    // X параллакс
    float offsetX = std::fmod((cc.x - WORLD_MID_X) * layer.parallaxX, (float)ts.x);
    if (offsetX < 0) offsetX += ts.x;

    // Y параллакс: нижний край слоя
    float renderBottomY = cc.y + cs.y * 0.5f;
    float renderTopY = renderBottomY - (float)ts.y;

    float startX = cc.x - cs.x * 0.5f - (float)ts.x;
    int   copies = (int)(cs.x / ts.x) + 3;

    Sprite spr(layer.texture);
    for (int i = 0; i < copies; ++i) {
        spr.setPosition(startX + i * (float)ts.x - offsetX, renderTopY);
        window.draw(spr);
    }
}

void Background::drawClouds(RenderWindow& window, const View&) {
    for (auto& c : clouds)
        window.draw(c.sprite);
}

void Background::render(RenderWindow& window, const View& camera) {
    window.setView(camera);
    drawSky(window, camera);
    for (auto& layer : layers)
        drawLayer(window, camera, layer);
    drawClouds(window, camera);
}