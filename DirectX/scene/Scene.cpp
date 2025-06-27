#include "Scene.h"

#include "SceneSprite.h"
#include "sprite.h"

#include "system_timer.h"

int texId = -1;

void Scene::init()
{
    Sprite whiteSprite1(L"assets/white.png");
    whiteSprite1.init({{100, 100}})
               ->moveTo({800, 100}, 5.0, EaseType::Linear);
    spriteMap["white1"] = whiteSprite1;

    Sprite whiteSprite2(L"assets/white.png");
    whiteSprite2.init({{100, 200}})
               ->moveTo({800, 200}, 5.0, EaseType::Back);
    spriteMap["white2"] = whiteSprite2;

    Sprite whiteSprite3(L"assets/white.png");
    whiteSprite3.init({{100, 300}})
               ->moveTo({800, 300}, 5.0, EaseType::Bounce);
    spriteMap["white3"] = whiteSprite3;

    Sprite whiteSprite4(L"assets/white.png");
    whiteSprite4.init({{100, 400}})
               ->moveTo({800, 400}, 5.0, EaseType::EaseIn);
    spriteMap["white4"] = whiteSprite4;

    Sprite whiteSprite5(L"assets/white.png");
    whiteSprite5.init({{100, 500}})
               ->moveTo({800, 500}, 5.0, EaseType::EaseOut);
    spriteMap["white5"] = whiteSprite5;

    Sprite whiteSprite6(L"assets/white.png");
    whiteSprite6.init({{100, 600}})
               ->moveTo({800, 600}, 5.0, EaseType::EaseInOut);
    spriteMap["white6"] = whiteSprite6;
    
    Sprite whiteSprite7(L"assets/white.png");
    whiteSprite7.init({{100, 700}})
               ->moveTo({800, 700}, 5.0, EaseType::Elastic);
    spriteMap["white7"] = whiteSprite7;

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
