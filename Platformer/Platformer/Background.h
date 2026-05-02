#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
using namespace sf;

struct BgLayer {
    Texture texture;
    float   parallax;   // 0.0 = не движется, 1.0 = движется как камера
    float   scrollX;    // для облаков — автоскролл px/s
    float   opacity;    // 0-255
};

class Background {
public:
    // Загрузить слои. Порядок = от дальнего к ближнему.
    bool load(const std::string& folder);

    // Вызывать каждый кадр до рендера мира.
    void update(float dt);
    void render(RenderWindow& window, const View& camera);

private:
    std::vector<BgLayer> layers;
    Color skyTop = Color(100, 149, 237);  // cornflowerblue
    Color skyBottom = Color(176, 226, 255);

    void drawGradient(RenderWindow& window, const View& camera);
    void drawLayer(RenderWindow& window, const View& camera, BgLayer& layer);
};