#include "Background.h"
#include <cmath>

bool Background::load(const std::string& folder) {
    // Описание слоёв: файл, параллакс, автоскролл, прозрачность
    struct Desc { std::string file; float parallax; float scroll; float opacity; };
    std::vector<Desc> descs = {
        { folder + "/skies/Sky_back_mountain.png", 0.10f,  0.f,   255 },
        { folder + "/forest/forest_back.png", 0.25f,  0.f,   220 },
        { folder + "/forest/forest_mid.png", 0.40f,  0.f,   200 },
        { folder + "/forest/forest_sky.png",    0.05f, 20.f,   180 },  // облака сами едут
    };

    for (auto& d : descs) {
        BgLayer layer;
        if (!layer.texture.loadFromFile(d.file)) continue; // пропускаем если нет файла
        layer.texture.setRepeated(true);   // ← ключевое: текстура тайлится по X
        layer.parallax = d.parallax;
        layer.scrollX = d.scroll;
        layer.opacity = d.opacity;
        layers.push_back(std::move(layer));
    }
    return true;
}

void Background::update(float dt) {
    for (auto& l : layers)
        l.scrollX += l.scrollX * dt;  // накапливаем сдвиг облаков
}

// Градиентное небо — два треугольника поверх всего
void Background::drawGradient(RenderWindow& window, const View& cam) {
    Vector2f center = cam.getCenter();
    Vector2f half = cam.getSize() * 0.5f;
    float L = center.x - half.x;
    float R = center.x + half.x;
    float T = center.y - half.y;
    float B = center.y + half.y;

    // Интерполируем цвет сверху вниз
    VertexArray quad(Quads, 4);
    quad[0] = Vertex({ L, T }, skyTop);
    quad[1] = Vertex({ R, T }, skyTop);
    quad[2] = Vertex({ R, B }, skyBottom);
    quad[3] = Vertex({ L, B }, skyBottom);
    window.draw(quad);
}

void Background::drawLayer(RenderWindow& window, const View& cam, BgLayer& layer) {
    Vector2f center = cam.getCenter();
    Vector2f size = cam.getSize();
    Vector2f half = size * 0.5f;

    Vector2u texSize = layer.texture.getSize();

    // Параллакс: слой сдвигается медленнее камеры
    float bgX = center.x * layer.parallax;

    // Прижимаем низ слоя к нижней части экрана
    float screenBottom = center.y + half.y;
    float layerTop = screenBottom - (float)texSize.y;

    // Тайлим по X через setTextureRect с огромной шириной
    float totalW = size.x + (float)texSize.x;  // чуть шире экрана
    float startX = center.x - half.x - (float)texSize.x;

    // Вычисляем смещение тайлинга
    float offsetX = std::fmod(bgX + layer.scrollX, (float)texSize.x);

    Sprite spr(layer.texture);
    spr.setColor(Color(255, 255, 255, (sf::Uint8)layer.opacity));

    // Рисуем несколько копий текстуры, перекрывая весь экран по X
    int copies = (int)(totalW / texSize.x) + 2;
    for (int i = 0; i < copies; ++i) {
        float x = startX + i * (float)texSize.x - offsetX;
        spr.setPosition(x, layerTop);
        window.draw(spr);
    }
}

void Background::render(RenderWindow& window, const View& camera) {
    window.setView(camera);  // рисуем в мировых координатах

    drawGradient(window, camera);
    for (auto& l : layers)
        drawLayer(window, camera, l);
}