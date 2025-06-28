#include "SceneManager.h"

#include "debug_ostream.h"
#include "direct3d.h"
#include "sprite.h"
#include "sprite_anim.h"
#include "texture.h"

std::function<void(int, SpriteState)> spriteDraw = static_cast<void(*)(int, SpriteState)>(Sprite_Draw);
std::function<void(int, SpriteState)> animDraw = static_cast<void(*)(int, SpriteState)>(SpriteAnim_Draw);

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
    // for (auto& [key, path] : textureData)
    // {
    //     textureMap[key] = Texture_Load(path.c_str());
    // }

    for (auto& [key, data] : AssetsSetting)
    {
        if (auto path = std::get_if<std::wstring>(&data))
        {
            assetsMap[key] = {Texture_Load(path->c_str()), spriteDraw};
        }
        else if (auto animPatternData = std::get_if<AnimPatternDataWithPath>(&data))
        {
            int textureId = Texture_Load(animPatternData->path.c_str());
            animPatternData->patternData.m_TextureId = textureId;
            int patternId = SpriteAnim_RegisterPattern(textureId, animPatternData->patternData);
            int playerId = SpriteAnim_CreatePlayer(patternId);
            assetsMap[key] = {playerId, animDraw};
        }
    }


    SceneDefinition(config);
}

void SceneDefinition(SceneManagerConfig config)
{
    int SCREEN_HEIGHT = Direct3D_GetBackBufferHeight();
    int SCREEN_WIDTH = Direct3D_GetBackBufferWidth();

    // Title 1: Kooler Presents
    Sprite s1_bg;
    s1_bg.set_id(assetsMap[Asset::Background].id)
         .initBackground(BLACK);

    Sprite s1_stageOpen;
    s1_stageOpen.set_id(assetsMap[Asset::StageOpen].id)
                .initBackground(WHITE).delay(1).fadeTo(0, 1);

    std::string s1_title_text = " KOOLER\nPresents";
    Sprite s1_title;
    s1_title.initCenterTitle(s1_title_text, INVISIBLE_WHITE)
            .delay(1)
            .fadeTo(1, 2, EaseType::EaseInOut)
            .delay(1)
            .fadeTo(0, 2, EaseType::EaseInOut);

    Scene s1;
    s1.addSprite(s1_bg, assetsMap[Asset::Background].drawFunction);
    s1.addSprite(s1_title, makeTextDrawFunction(s1_title_text, config.titlePresentsText));
    s1.addSprite(s1_stageOpen, assetsMap[Asset::StageOpen].drawFunction);

    // RegisterScene(s1);

    // Scene 2: Running Man Title
    std::string s2_title_text = "RUNNING\n  MAN";
    Sprite s2_title;
    s2_title.initCenterTitle(s2_title_text, INVISIBLE_WHITE)
            .delay(1)
            .fadeTo(1, 2, EaseType::EaseIn)
            .delay(4.5)
            .fadeTo(0, 1.5, EaseType::EaseOut);

    Sprite s2_bg;
    s2_bg.set_id(assetsMap[Asset::BackgroundStar].id)
         .initBackground(INVISIBLE_WHITE)
         .fadeTo(1, 1, EaseType::EaseOut)
         .delay(4.5)
         .fadeTo(0, 2, EaseType::EaseOut);

    Sprite s2_running000(assetsMap[Asset::RunningMan000].id);
    s2_running000
        .init({{100, 200}, GetScreenCenterPosition({100, 200}, SCREEN_WIDTH, SCREEN_HEIGHT), {0, 0}})
        .delay(1)
        .scaleTo({1.5, 1.5}, 3, EaseType::EaseIn)
        .delay(0.5)
        .beginParallel()
        .scaleTo({0.75, 0.75}, 3, EaseType::EaseOut)
        .moveTo({1800, 550}, 3, EaseType::EaseOut)
        .endParallel();

    Sprite s2_background;
    s2_background.set_id(assetsMap[Asset::BackgroundJapan].id)
                 .init({{4000.0f, 900.0f}, {-2400.0f, 0.0f}, {1.0f, 1.0f}, 0.0f, INVISIBLE_WHITE})
                 .delay(5.5)
                 .fadeTo(1, 2, EaseType::EaseIn);
    Sprite s2_foregroundJapan;
    s2_foregroundJapan.set_id(assetsMap[Asset::ForegroundJapan].id)
                      .init({{4000.0f, 900.0f}, {-2400.0f, 0.0f}, {1.0f, 1.0f}, 0.0f, INVISIBLE_WHITE})
                      .delay(5.5)
                      .fadeTo(1, 1.75, EaseType::EaseIn);
    Sprite s2_ground;
    s2_ground.set_id(assetsMap[Asset::Ground].id)
             .init({{4000.0f, 900.0f}, {-2400.0f, 0.0f}, {1.0f, 1.0f}, 0.0f, INVISIBLE_WHITE})
             .delay(5.5)
             .fadeTo(1, 1.5, EaseType::EaseIn);

    Scene s2;
    s2.addSprite(s1_bg, assetsMap[Asset::Background].drawFunction);
    s2.addSprite(s2_bg, assetsMap[Asset::BackgroundStar].drawFunction);
    s2.addSprite(s2_running000, assetsMap[Asset::RunningMan000].drawFunction);
    s2.addSprite(s2_background, assetsMap[Asset::BackgroundJapan].drawFunction);
    s2.addSprite(s2_foregroundJapan, assetsMap[Asset::ForegroundJapan].drawFunction);
    s2.addSprite(s2_ground, assetsMap[Asset::Ground].drawFunction);
    s2.addSprite(s2_title, makeTextDrawFunction(s2_title_text, config.runningmanText));
    // RegisterScene(s2);
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

DirectX::XMFLOAT2 GetScreenCenterPosition(const DirectX::XMFLOAT2& size, float screenWidth, float screenHeight)
{
    return DirectX::XMFLOAT2{
        (screenWidth - size.x) * 0.5f,
        (screenHeight - size.y) * 0.5f
    };
}
