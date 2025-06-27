#include "Scene.h"

#include "SceneSprite.h"
#include "sprite.h"

#include "system_timer.h"

int texId = -1;

void Scene::init()
{
    Sprite whiteSprite(L"assets/white.png");
    whiteSprite.init({{100, 200}, {1, 1}, 0.0f, 1.0f, {1, 1, 1, 1}})
               ->moveTo({400, 300}, 2.0) // Move right once
               ->scaleTo({1.5f, 1.5f}, 2) // Scale up
               ->rotateTo(45.0f, 1.0f) // Rotate
               ->fadeTo(0.5f, 2.0f) // Fade
               ->moveTo({300, 400}, 1.0); // Move down once at the end
    spriteMap["white"] = whiteSprite;

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
