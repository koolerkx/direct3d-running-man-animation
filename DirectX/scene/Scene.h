#pragma once
#include <map>
#include <string>

#include "SceneSprite.h"

class Scene
{
public:
    bool is_end;

    void init();

    void startSetup();
    void draw_loop();
private:
    double startTime = 0;
    double timeOffset = 0;
    double duration = 10;

    std::map<std::string, Sprite> spriteMap;
};
