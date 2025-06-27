#include "Scene.h"

#include "debug_ostream.h"
#include "SceneManager.h"
#include "SceneSprite.h"

#include "system_timer.h"


void Scene::init()
{
}

void Scene::startSetup()
{
    // get max duration
    duration = 0.0;
    for (SpriteWithDrawFunction& sprite : sprites)
    {
        double spriteDuration = sprite.sprite.getDuration();
        if (spriteDuration > duration)
        {
            duration = spriteDuration;
        }
    }
    hal::dout << duration << std::endl;

    startTime = SystemTimer_GetTime();
}

void Scene::draw_loop()
{
    double now = SystemTimer_GetTime();
    timeOffset = now - startTime;

    for (SpriteWithDrawFunction& sprite : sprites)
    {
        sprite.drawFunction(sprite.sprite.drawingId, sprite.sprite.getState(timeOffset));
    }

    if (timeOffset >= duration)
    {
        is_end = true;
    }
}

void Scene::addSprite(Sprite sprite, std::function<void(int, SpriteState)> drawFunction)
{
    sprites.push_back({sprite, drawFunction});
}
