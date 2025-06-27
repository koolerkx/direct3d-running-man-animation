#include "Scene.h"

#include "SceneSprite.h"
#include "sprite.h"

#include "system_timer.h"

int texId = -1;

void Scene::init()
{
    // Test 1: Simple repeat group
    Sprite whiteSprite1(L"assets/white.png");
    whiteSprite1.init({{100, 100}})
                ->moveTo({400, 100}, 1.0) // Move right once
                ->beginRepeat(RepeatMode::PingPong, 3) // Start repeat group
                ->scaleTo({1.5f, 1.5f}, 0.5) // Scale up
                ->rotateTo(45.0f, 0.5) // Rotate
                ->fadeTo(0.5f, 0.5) // Fade
                ->endRepeat() // End repeat (will ping-pong 3 times)
                ->moveTo({400, 400}, 1.0); // Move down once at the end
    spriteMap["white1"] = whiteSprite1;

    // Test 2: Normal repeat mode
    Sprite whiteSprite2(L"assets/white.png");
    whiteSprite2.init({{500, 100}})
                ->beginRepeat(RepeatMode::Normal, 2) // Normal repeat 2 times
                ->moveTo({600, 100}, 0.5)
                ->moveTo({600, 200}, 0.5)
                ->moveTo({500, 200}, 0.5)
                ->moveTo({500, 100}, 0.5)
                ->endRepeat()
                ->scaleTo({2.0f, 2.0f}, 1.0);
    spriteMap["white2"] = whiteSprite2;

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
