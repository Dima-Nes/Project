#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "World.h"

using namespace sf;

struct BgLayer {
    Texture texture;
    float   parallaxX = 0.f;
    float   parallaxY = 0.f;
};

struct CloudSprite {
    Sprite sprite;
    float  speed, x, y, scale;
};

class Background {
public:
    bool load(const std::string& folder, const std::string& cloudFolder);
    void update(float dt, const View& camera);
    void render(RenderWindow& window, const View& camera);

private:
    Texture skyTex;
    bool    hasSky = false;

    std::vector<BgLayer>     layers;
    std::vector<Texture>     cloudTextures;
    std::vector<CloudSprite> clouds;
    bool cloudsInited = false;

    static constexpr int CLOUD_COUNT = 12;

    void drawSky(RenderWindow& window, const View& camera);
    void drawLayer(RenderWindow& window, const View& camera, const BgLayer& layer);
    void drawClouds(RenderWindow& window, const View& camera);
    void initClouds(const View& camera);
    void wrapCloud(CloudSprite& c, const View& camera);
};