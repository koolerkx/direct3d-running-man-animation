#include "Scene.h"

#include "debug_ostream.h"
#include "SceneSprite.h"

#include "system_timer.h"

void Scene::init()
{
    spriteMap["background"] = Sprite(L"assets/white.png");

    spriteMap["background"]
        .init({{300, 300}, {256, 256}, {1, 1, 1, 0}, 0})
        ->delay(5)->fadeInOut(0, 1, 10.0);

    // sum up the duration in sprite map
    duration = 20;
}

void Scene::startSetup()
{
    startTime = SystemTimer_GetTime();
}

void Scene::draw_loop()
{
    double now = SystemTimer_GetTime();
    timeOffset = now - startTime;

    spriteMap["background"].draw(timeOffset);

    if (timeOffset >= duration)
    {
        is_end = true;
    }
}
