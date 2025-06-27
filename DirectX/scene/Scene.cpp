#include "Scene.h"

#include "SceneSprite.h"
#include "sprite.h"

#include "system_timer.h"

int texId = -1;

void Scene::init()
{
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

    for (auto& [key, sprite] : spriteMap)
    {
        sprite.draw(timeOffset);
    }

    if (timeOffset >= duration)
    {
        is_end = true;
    }
}
