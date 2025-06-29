#pragma once
#include <functional>
#include <vector>

#include "SceneSprite.h"

struct SpriteWithDrawFunction
{
    ScreenScprite sprite;
    std::function<void(int, SpriteState)>drawFunction;
};

class Scene
{
public:
    bool is_end = false;

    void init();

    void startSetup();
    void draw_loop();

    void addSprite(ScreenScprite sprite, std::function<void(int, SpriteState)> drawFunction);
private:
    double startTime = 0;
    double timeOffset = 0;
    double duration = -1;

    std::vector<SpriteWithDrawFunction> sprites;
};
