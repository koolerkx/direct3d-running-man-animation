#include "SceneManager.h"

#include "debug_ostream.h"
#include "sprite.h"
#include "texture.h"

std::function<void(int, SpriteState)> spriteDraw = static_cast<void(*)(int, SpriteState)>(Sprite_Draw);

std::function<void(int, SpriteState)> makeTextDrawFunction(const std::string& str, hal::DebugText* text)
{
    return [str, text](int, SpriteState ss) mutable
    {
        text->SetText(str.c_str());
        text->Draw(ss);
        text->Clear();
    };
}

void SceneInitialize(SceneManagerConfig config)
{
    // テクスチャ初期化
    for (auto& [key, path] : texturePaths)
    {
        textureMap[key] = Texture_Load(path.c_str());
    }

    SceneDefinition(config);
}

void SceneDefinition(SceneManagerConfig config)
{
    // Title 1: Kooler Presents
    Sprite s1_bg;
    s1_bg.set_id(textureMap[Texture::Background])
         ->initBackground(BLACK);

    std::string s1_title_text = " KOOLER\nPresents";
    Sprite s1_title;
    s1_title.initCenterTitle(s1_title_text, INVISIBLE_WHITE)
            ->delay(1)
            ->fadeTo(1, 2, EaseType::EaseInOut)
            ->delay(1)
            ->fadeTo(0, 2, EaseType::EaseInOut);

    Scene s1;
    s1.addSprite(s1_bg, spriteDraw);
    s1.addSprite(s1_title, makeTextDrawFunction(s1_title_text, config.titlePresentsText));

    // RegisterScene(s1);


    Scene s2;
    std::string s2_title_text = "RUNNING\n  MAN";
    Sprite s2_title;
    s2_title.initCenterTitle(s2_title_text, INVISIBLE_WHITE)
            ->fadeTo(1, 2, EaseType::EaseInOut)
            ->delay(5);
    
    s2.addSprite(s1_bg, spriteDraw);
    s2.addSprite(s2_title, makeTextDrawFunction(s2_title_text, config.runningmanText));
    RegisterScene(s2);
}

void SceneLoop()
{
    if (isPrevSceneEnd)
    {
        scenes[sceneIndex].startSetup();
        isPrevSceneEnd = false;
    }

    scenes[sceneIndex].draw_loop();
    if (scenes[sceneIndex].is_end && sceneIndex < (scenes.size() - 1))
    {
        sceneIndex++;
        isPrevSceneEnd = true;
    }
}

void RegisterScene(Scene scene)
{
    scenes.emplace_back(scene);
}
