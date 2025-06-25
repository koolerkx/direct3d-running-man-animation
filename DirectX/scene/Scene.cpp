#include "Scene.h"
#include "SceneSprite.h"

#include "system_timer.h"

void Scene::init()
{
    spriteMap["background"] = Sprite(L"assets/white.png");
    
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
    
    if(timeOffset >= duration) {
        is_end = true;
    }
}
