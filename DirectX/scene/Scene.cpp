/**
 * @file Scene.cpp
 * @brief シーンクラスの実装
 * @details スプライトをまとめて、管理するクラス
 * @author KOOLER FAN
 * @date 2025/06/30
 */

#include "Scene.h"

#include "debug_ostream.h"
#include "SceneSprite.h"

#include "system_timer.h"


void Scene::init()
{
}

void Scene::startSetup()
{
    // シーンの長さを取得
    duration = 0.0;
    for (SpriteWithDrawFunction& sprite : sprites)
    {
        double spriteDuration = sprite.sprite.getDuration();
        if (spriteDuration > duration)
        {
            duration = spriteDuration;
        }
    }

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

void Scene::addSprite(SceneSprite sprite, std::function<void(int, SpriteState)> drawFunction)
{
    sprites.push_back({sprite, drawFunction});
}
