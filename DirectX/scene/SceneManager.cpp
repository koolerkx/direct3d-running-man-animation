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
        .scaleTo({2, 2}, 3, EaseType::EaseIn)
        .delay(0.5)
        .beginParallel()
        .scaleTo({0.75, 0.75}, 3, EaseType::EaseOut)
        .moveTo({1800, 550}, 3, EaseType::EaseOut)
        .endParallel();

    Sprite s2_background;
    s2_background.set_id(assetsMap[Asset::BackgroundJapan].id)
                 .init({{4000.0f, 900.0f}, {-1600.0f, 0.0f}, {1.0f, 1.0f}, 0.0f, INVISIBLE_WHITE})
                 .delay(5.5)
                 .fadeTo(1, 2, EaseType::EaseIn);
    Sprite s2_foregroundJapan;
    s2_foregroundJapan.set_id(assetsMap[Asset::ForegroundJapan].id)
                      .init({{4000.0f, 900.0f}, {-2000.0f, 0.0f}, {1.0f, 1.0f}, 0.0f, INVISIBLE_WHITE})
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

    // Scene 3
    Sprite s3_background;
    s3_background.set_id(assetsMap[Asset::BackgroundJapan].id)
                 .init({{4000.0f, 900.0f}, {-1600.0f, 0.0f}, {1.0f, 1.0f}, 0.0f, WHITE})
                 .delay(1)
                 .moveTo({-1200, 0}, 11);
    Sprite s3_foregroundJapan;
    s3_foregroundJapan.set_id(assetsMap[Asset::ForegroundJapan].id)
                      .init({{4000.0f, 900.0f}, {-2000.0f, 0.0f}, {1.0f, 1.0f}, 0.0f, WHITE})
                      .delay(1)
                      .moveTo({-1200, 0}, 11);

    Sprite s3_ground;
    s3_ground.set_id(assetsMap[Asset::Ground].id)
             .init({{4000.0f, 900.0f}, {-2400.0f, 0.0f}, {1.0f, 1.0f}, 0.0f, WHITE})
             .delay(1)
             .moveTo({-1200, 0}, 11);

    Sprite s3_running003;
    s3_running003.set_id(assetsMap[Asset::RunningMan003].id)
                 .init({{210, 300}, {1700.0f, 500.0f}, {1.0f, 1.0f}, 0.0f, WHITE})
                 .moveTo({(SCREEN_WIDTH - 210) / 2.0f, 500}, 12);

    Sprite s3_enemy01;
    s3_enemy01.set_id(assetsMap[Asset::Enemy1].id)
              .init({{384, 384}, {(SCREEN_WIDTH - 384) / 2.0f - 400, -100.0f}, {0.0f, 0.0f}, 0.0f, WHITE})
              .delay(1)
              .beginParallel()
              .scaleTo({1.0, 1.0}, 8)
              .moveTo({(SCREEN_WIDTH - 384) / 2.0f, 0}, 7, EaseType::Bounce)
              .endParallel();

    Sprite s3_skill01;
    s3_skill01.set_id(assetsMap[Asset::Skill1].id)
              .init({{1600, 1200}, {(SCREEN_WIDTH - 1600) / 2.0f, -250}, {1.0f, 1.0f}, 0.0f, INVISIBLE_WHITE})
              .delay(9)
              .fadeTo(1, 0)
              .delay(2.3)
              .fadeTo(0, 0);

    Sprite s3_explo02;
    s3_explo02.set_id(assetsMap[Asset::Explosion2].id)
              .init({
                  {static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT)}, {0, 0}, {1, 1}, 0.0f,
                  INVISIBLE_WHITE
              })
              .delay(11.7)
              .fadeTo(1, 0)
              .delay(0.48)
              .fadeTo(0, 0);

    Scene s3;
    s3.addSprite(s3_background, assetsMap[Asset::BackgroundJapan].drawFunction);
    s3.addSprite(s3_foregroundJapan, assetsMap[Asset::ForegroundJapan].drawFunction);

    s3.addSprite(s3_enemy01, assetsMap[Asset::Enemy1].drawFunction);
    s3.addSprite(s3_skill01, assetsMap[Asset::Skill1].drawFunction);

    s3.addSprite(s3_ground, assetsMap[Asset::Ground].drawFunction);
    s3.addSprite(s3_running003, assetsMap[Asset::RunningMan003].drawFunction);

    s3.addSprite(s3_explo02, assetsMap[Asset::Explosion2].drawFunction);

    // Scene 4
    Sprite s4_white;
    s4_white.set_id(assetsMap[Asset::Background].id)
            .initBackground(WHITE)
            .fadeTo(0, 0.5, EaseType::EaseIn)
            .delay(10.5)
            .colorTo(INVISIBLE_BLACK, 0)
            .fadeTo(1, 0.5, EaseType::EaseIn);

    Sprite s4_background;
    s4_background.set_id(assetsMap[Asset::BackgroundWorldEnd].id)
                 .init({{4000.0f, 900.0f}, {-1200.0f, 0.0f}, {1.0f, 1.0f}, 0.0f, WHITE})
                 .moveTo({-800, 0}, 11);

    Sprite s4_foregroundJapan;
    s4_foregroundJapan.set_id(assetsMap[Asset::ForegroundJapan].id)
                      .init({{4000.0f, 900.0f}, {-1200.0f, 0.0f}, {1.0f, 1.0f}, 0.0f, WHITE})
                      .moveTo({-400, 0}, 11);

    Sprite s4_ground;
    s4_ground.set_id(assetsMap[Asset::Ground].id)
             .init({{4000.0f, 900.0f}, {-1200.0f, 0.0f}, {1.0f, 1.0f}, 0.0f, WHITE})
             .moveTo({0, 0}, 11);

    Sprite s4_running003;
    s4_running003.set_id(assetsMap[Asset::RunningMan003].id)
                 .init({{210, 300}, {700, 500}, {1.0f, 1.0f}, 0.0f, WHITE})
                 .moveTo({400, 500}, 6)
                 .beginParallel()
                 .scaleTo({0.0f, 0.0f}, 5, EaseType::EaseIn)
                 .rotateTo(DirectX::XMConvertToRadians(-900), 5, EaseType::EaseInOut)
                 .moveTo({400, 450}, 5)
                 .endParallel();

    Sprite s4_door;
    s4_door.set_id(assetsMap[Asset::Door].id)
           .init({{320, 240}, {-300, 500}, {1.0f, 1.0f}, DirectX::XMConvertToRadians(90.0f), WHITE})
           .delay(4)
           .beginParallel()
           .moveTo({400, 500}, 7)
           .endParallel();

    Sprite s4_ufo1;
    s4_ufo1.set_id(assetsMap[Asset::UFO].id)
           .init({{150, 150}, {-100, 100}, {1.0f, 1.0f}, 0, NEON})
           .beginParallel()
           .moveTo({500, 150}, 1.5, EaseType::Elastic)
           .rotateTo(DirectX::XMConvertToRadians(-25), 2, EaseType::EaseIn)
           .endParallel()
           .beginParallel()
           .moveTo({700, 300}, 2.5, EaseType::Back)
           .rotateTo(DirectX::XMConvertToRadians(45), 2.5, EaseType::Bounce)
           .endParallel()
           .beginParallel()
           .moveTo({900, 50}, 1.5, EaseType::EaseOut)
           .rotateTo(DirectX::XMConvertToRadians(-15), 1.5, EaseType::EaseInOut)
           .endParallel()
           .moveTo({1700, 400}, 1, EaseType::Back)
           .rotateTo(DirectX::XMConvertToRadians(-30), 2, EaseType::Elastic)
           .endParallel();

    Sprite s4_ufo2;
    s4_ufo2.set_id(assetsMap[Asset::UFO].id)
           .init({{150, 150}, {-100, 300}, {1.0f, 1.0f}, 0, AQUA})
           .beginParallel()
           .moveTo({400, 250}, 2, EaseType::Back)
           .rotateTo(DirectX::XMConvertToRadians(-45), 2, EaseType::EaseInOut)
           .endParallel()
           .beginParallel()
           .moveTo({650, 400}, 3, EaseType::Bounce)
           .rotateTo(DirectX::XMConvertToRadians(25), 1.5, EaseType::Elastic)
           .endParallel()
           .beginParallel()
           .moveTo({1000, 200}, 1, EaseType::EaseIn)
           .rotateTo(DirectX::XMConvertToRadians(-15), 2, EaseType::Elastic)
           .endParallel()
           .beginParallel()
           .moveTo({1700, 0}, 1, EaseType::EaseInOut)
           .rotateTo(DirectX::XMConvertToRadians(30), 2, EaseType::Elastic)
           .endParallel();


    Scene s4;
    s4.addSprite(s4_background, assetsMap[Asset::BackgroundWorldEnd].drawFunction);
    s4.addSprite(s4_foregroundJapan, assetsMap[Asset::ForegroundJapan].drawFunction);
    s4.addSprite(s4_ground, assetsMap[Asset::Ground].drawFunction);

    s4.addSprite(s4_ufo1, assetsMap[Asset::UFO].drawFunction);
    s4.addSprite(s4_ufo2, assetsMap[Asset::UFO].drawFunction);

    s4.addSprite(s4_door, assetsMap[Asset::Door].drawFunction);

    s4.addSprite(s4_running003, assetsMap[Asset::RunningMan003].drawFunction);

    s4.addSprite(s4_white, assetsMap[Asset::Background].drawFunction);

    // Scene 5
    Scene s5;

    Sprite s5_white;
    s5_white.set_id(assetsMap[Asset::Background].id)
            .initBackground(BLACK)
            .fadeTo(0, 0.5, EaseType::EaseIn);

    Sprite s5_fg;
    s5_fg.set_id(assetsMap[Asset::ForegroundShinjyuku].id)
         .init({{2000.0f, 950.0f}, {-400.0f, SCREEN_HEIGHT - 950.0f}, {1.0f, 1.0f}, 0.0f, WHITE})
         .moveTo({0, SCREEN_HEIGHT - 950.0f}, 3)
         .delay(7)
         .moveTo({-1600, SCREEN_HEIGHT - 800.0f}, 3)
         .fadeTo(0, 0);

    Sprite s5_bg_1;
    s5_bg_1.set_id(assetsMap[Asset::BackgroundShinjyuku01].id)
           .init({{1600.0f, 950.0f}, {0.0f, .0f}, {1.0f, 1.0f}, 0.0f, WHITE})
           .delay(10)
           .moveTo({-1600, 0}, 3)
           .fadeTo(0, 0);

    Sprite s5_bg_2;
    s5_bg_2.set_id(assetsMap[Asset::BackgroundShinjyuku02].id)
           .init({{1700.0f, 850.0f}, {-50.0f, 5.0f}, {1.0f, 1.0f}, 0.0f, WHITE})
           .moveTo({0, 5}, 3)
           .delay(7)
           .moveTo({-1600, 5}, 3)
           .fadeTo(0, 0);

    Sprite s5_running003;
    s5_running003.set_id(assetsMap[Asset::RunningMan003].id)
                 .init({{210, 300}, {1400, -300}, {1.0f, 1.0f}, DirectX::XMConvertToRadians(900), WHITE})
                 .beginParallel()
                 .rotateTo(0, 2)
                 .moveTo({1400, 800}, 2)
                 .endParallel()
                 .beginParallel()
                 .moveTo({1200, 400}, 2)
                 .scaleTo({3, 3}, 2, EaseType::EaseOut)
                 .endParallel()
                 .moveTo({900, 400}, 2.5)
                 .beginParallel()
                 .rotateTo(DirectX::XMConvertToRadians(90), 1, EaseType::EaseOut)
                 .moveTo({1200, 1000}, 2)
                 .endParallel()
                 .fadeTo(0, 0);

    Sprite s5_train_first;
    s5_train_first.set_id(assetsMap[Asset::TrainFirst].id)
                  .init({{800, 800}, {-800, 0}, {1.0f, 1.0f}, 0, WHITE})
                  .delay(5)
                  .moveTo({800, 0}, 3, EaseType::EaseInOut)
                  .delay(2)
                  .moveTo({-2000, 0}, 3);

    Sprite s5_train_mid;
    s5_train_mid.set_id(assetsMap[Asset::TrainMid].id)
                .init({{800, 800}, {-1550, -55}, {1.0f, 1.0f}, 0, WHITE})
                .delay(5)
                .moveTo({0, -55}, 3, EaseType::EaseInOut)
                .delay(2)
                .moveTo({-2000, -55}, 3);

    Sprite s5_train_door_left;
    s5_train_door_left.set_id(assetsMap[Asset::TrainDoor].id)
                      .init({{800, 900}, {-800, 0}, {1.0f, 1.0f}, 0, BLACK})
                      .delay(8.5)
                      .moveTo({0, 0}, 1.5, EaseType::Bounce)
                      .delay(3)
                      .moveTo({-800, 0}, 1.5, EaseType::Bounce);

    Sprite s5_train_door_right;
    s5_train_door_right.set_id(assetsMap[Asset::TrainDoor].id)
                       .init({{800, 900}, {1600, 0}, {1.0f, 1.0f}, 0, BLACK, true})
                       .delay(8.5)
                       .moveTo({800, 0}, 1.5, EaseType::Bounce)
                       .delay(3)
                       .moveTo({1600, 0}, 1.5, EaseType::Bounce);

    Sprite s5_laser_background;
    
    s5_laser_background.set_id(assetsMap[Asset::LaserBackground].id)
                       .initBackground(WHITE)
                       .delay(15).fadeTo(0, 2);
    
    s5.addSprite(s5_laser_background, assetsMap[Asset::LaserBackground].drawFunction);
    s5.addSprite(s5_bg_1, assetsMap[Asset::BackgroundShinjyuku01].drawFunction);
    s5.addSprite(s5_bg_2, assetsMap[Asset::BackgroundShinjyuku02].drawFunction);

    s5.addSprite(s5_running003, assetsMap[Asset::RunningMan003].drawFunction);

    s5.addSprite(s5_train_first, assetsMap[Asset::TrainFirst].drawFunction);
    s5.addSprite(s5_train_mid, assetsMap[Asset::TrainMid].drawFunction);

    s5.addSprite(s5_fg, assetsMap[Asset::ForegroundShinjyuku].drawFunction);

    s5.addSprite(s5_train_door_left, assetsMap[Asset::TrainDoor].drawFunction);
    s5.addSprite(s5_train_door_right, assetsMap[Asset::TrainDoor].drawFunction);

    s5.addSprite(s5_white, assetsMap[Asset::ForegroundShinjyuku].drawFunction);

    // Scene 6
    Scene s6;

    Sprite s6_ground;
    s6_ground.set_id(assetsMap[Asset::Ground].id)
             .init({{4000.0f, 900.0f}, {-2400.0f, 200.0f}, {1.0f, 1.0f}, 0.0f, WHITE, true})
             .moveTo({-2400.0f, 0}, 1, EaseType::Bounce)
             .delay(0.5)
             .moveTo({0, 0}, 20)
             .moveTo({-1200, 0}, 3.5);

    Sprite s6_penguin;
    s6_penguin.set_id(assetsMap[Asset::Penguin].id)
              .init({{200.0f, 200.0f}, {700, -200.0f}, {1.0f, 1.0f}, 0.0f, WHITE, false})
              .moveTo({700, 100}, 1, EaseType::Bounce)
              .delay(0.5)
              .moveTo({500, 100}, 2, EaseType::EaseIn).flip()
              .moveTo({800, 120}, 2, EaseType::Back).flip()
              .moveTo({400, 80}, 2, EaseType::EaseOut).flip()
              .moveTo({900, 100}, 2, EaseType::EaseInOut).flip()
              .moveTo({500, 120}, 2, EaseType::Elastic).flip()
              .moveTo({800, 80}, 2, EaseType::Bounce).flip()
              .moveTo({400, 100}, 2, EaseType::EaseIn).flip()
              .moveTo({900, 120}, 2, EaseType::EaseInOut).flip()
              .moveTo({500, 80}, 2, EaseType::Back).flip()
              .moveTo({800, 100}, 2, EaseType::Elastic).flip()
              .moveTo({400, 120}, 2, EaseType::EaseOut).flip()
              .moveTo({900, 120}, 1.5, EaseType::Back).flip()
              .beginParallel()
              .moveTo({700, 200}, 1)
              .rotateTo(DirectX::XMConvertToRadians(360), 1)
              .scaleTo({0, 0}, 1, EaseType::EaseOut)
              .endParallel();

    Sprite s6_running003;
    s6_running003.set_id(assetsMap[Asset::RunningMan003].id)
                 .init({{210, 300}, {1600, 500}, {1.0f, 1.0f}, 0, WHITE})
                 .moveTo({1200, 500}, 1, EaseType::Elastic)
                 .moveTo({900, 500}, 2, EaseType::EaseInOut)
                 .moveTo({1100, 500}, 2, EaseType::EaseInOut)
                 .fadeTo(0, 0.6);

    Sprite s6_running001;
    s6_running001.set_id(assetsMap[Asset::RunningMan001].id)
                 .init({{210, 300}, {1100, 500}, {1.0f, 1.0f}, 0, INVISIBLE_WHITE})
                 .delay(5.4).fadeTo(1, 0.6)
                 .moveTo({1400, 500}, 2, EaseType::EaseIn)
                 .moveTo({800, 500}, 2, EaseType::EaseInOut)
                 .moveTo({1000, 500}, 2, EaseType::EaseOut)
                 .delay(1)
                 .fadeTo(0, 0);

    Sprite s6_running001_rainbowStroke;
    s6_running001_rainbowStroke.set_id(assetsMap[Asset::RunningMan001].id)
                               .init({{210, 300}, {1000, 500}, {1.0f, 1.0f}, 0, INVISIBLE_WHITE})
                               .ShaderTo(ShaderType::RainbowStroke)
                               .delay(12.4).fadeTo(1, 0.6)
                               .moveTo({700, 500}, 2, EaseType::EaseIn)
                               .moveTo({900, 500}, 2, EaseType::EaseInOut)
                               .moveTo({500, 500}, 2, EaseType::EaseOut)
                               .delay(1)
                               .fadeTo(0, 0);

    Sprite s6_running001_rainbowTexture;
    s6_running001_rainbowTexture.set_id(assetsMap[Asset::RunningMan001].id)
                                .init({{210, 300}, {500, 500}, {1.0f, 1.0f}, 0, INVISIBLE_WHITE})
                                .ShaderTo(ShaderType::RainbowTexture)
                                .delay(19).fadeTo(1, 1)
                                .moveTo({300, 500}, 2, EaseType::EaseOut)
                                .flip()
                                .moveTo({700, 200}, 2, EaseType::EaseInOut)
                                .delay(0.2).flip().delay(0.2).flip().delay(0.2).flip().delay(0.2).flip().delay(0.2).
                                flip()
                                .beginParallel()
                                .rotateTo(DirectX::XMConvertToRadians(360), 1)
                                .scaleTo({0, 0}, 1, EaseType::EaseOut)
                                .endParallel();

    Sprite s6_apple;
    s6_apple.set_id(assetsMap[Asset::Apple].id)
            .init({{100, 100}, {750, 200}, {1.0f, 1.0f}, 0, INVISIBLE_WHITE})
            .delay(1.5)
            .fadeTo(1, 1)
            .beginParallel()
            .moveTo({1100, 500}, 2.5, EaseType::EaseInOut)
            .rotateTo(DirectX::XMConvertToRadians(5400), 2.5, EaseType::Back)
            .endParallel()
            .fadeTo(0, 0);

    Sprite s6_banana;
    s6_banana.set_id(assetsMap[Asset::Banana].id)
             .init({{100, 100}, {750, 200}, {1.0f, 1.0f}, 0, INVISIBLE_WHITE})
             .delay(8)
             .fadeTo(1, 1)
             .beginParallel()
             .moveTo({1000, 500}, 3, EaseType::EaseInOut)
             .rotateTo(DirectX::XMConvertToRadians(-5400), 3, EaseType::Back)
             .endParallel()
             .fadeTo(0, 0);

    Sprite s6_suica;
    s6_suica.set_id(assetsMap[Asset::Suica].id)
            .init({{100, 100}, {750, 200}, {1.0f, 1.0f}, 0, INVISIBLE_WHITE})
            .delay(15)
            .fadeTo(1, 1)
            .beginParallel()
            .moveTo({500, 500}, 3, EaseType::EaseInOut)
            .rotateTo(DirectX::XMConvertToRadians(9000), 3, EaseType::Back)
            .endParallel()
            .fadeTo(0, 0);

    Sprite s6_henshin;
    s6_henshin.set_id(assetsMap[Asset::Henshin].id)
              .init({{400, 400}, {1000, 400}, {1.0f, 1.0f}, 0, INVISIBLE_WHITE})
              .delay(5)
              .fadeTo(1, 0)
              .delay(1)
              .fadeTo(0, 0)
              .moveTo({900, 400}, 0)
              .delay(6)
              .fadeTo(1, 0)
              .delay(1)
              .fadeTo(0, 0)
              .moveTo({400, 400}, 0)
              .delay(6)
              .fadeTo(1, 0)
              .delay(1)
              .fadeTo(0, 0);

    Sprite s6_door;
    s6_door.set_id(assetsMap[Asset::Door].id)
           .init({{640, 480}, {480, 0}, {1.0f, 1.0f}, 0, INVISIBLE_WHITE})
           .delay(25)
           .fadeTo(1, 1);

    Sprite s6_map;
    s6_map.set_id(assetsMap[Asset::Map].id)
          .init({{600, 480}, {480, 0}, {0.0f, 0.0f}, 0, INVISIBLE_WHITE})
          .delay(25)
          .fadeTo(1, 1)
          .delay(0.5)
          .beginParallel()
          .moveTo({500, 150}, 1, EaseType::EaseInOut)
          .scaleTo({2.0f, 2.0f}, 1, EaseType::EaseInOut)
          .endParallel()
          .delay(0.5)
          .scaleTo({4.0f, 4.0f}, 1, EaseType::EaseInOut);

    Sprite s6_meteor01;
    s6_meteor01.set_id(assetsMap[Asset::Meteor01].id)
               .init({{120, 120}, {100, 50}, {1.0f, 1.0f}, 0, INVISIBLE_WHITE, true})
               .fadeTo(1, 0.5)
               .beginParallel()
               .scaleTo({1.5, 1.5}, 1)
               .moveTo({300, 250}, 2, EaseType::EaseOut)
               .fadeTo(0, 2)
               .endParallel()
               .delay(4)
               .moveTo({100, 50}, 0).scaleTo({1.0f, 1.0f}, 0)
               .fadeTo(1, 0.5)
               .beginParallel()
               .scaleTo({1.5, 1.5}, 1)
               .moveTo({300, 250}, 2, EaseType::EaseOut)
               .fadeTo(0, 2)
               .endParallel();

    Sprite s6_meteor02;
    s6_meteor02.set_id(assetsMap[Asset::Meteor02].id)
               .init({{120, 120}, {1200, 0}, {1.0f, 1.0f}, 0, INVISIBLE_WHITE, true})
               .delay(0.5)
               .fadeTo(1, 0.5)
               .beginParallel()
               .scaleTo({1.5, 1.5}, 1)
               .moveTo({1400, 200}, 2, EaseType::EaseOut)
               .fadeTo(0, 2)
               .endParallel()
               .delay(4)
               .moveTo({1200, 0}, 0).scaleTo({1.0f, 1.0f}, 0)
               .fadeTo(1, 0.5)
               .beginParallel()
               .scaleTo({1.5, 1.5}, 1)
               .moveTo({1400, 200}, 2, EaseType::EaseOut)
               .fadeTo(0, 2)
               .endParallel();

    Sprite s6_meteor03;
    s6_meteor03.set_id(assetsMap[Asset::Meteor03].id)
               .init({{150, 150}, {900, 100}, {1.0f, 1.0f}, 0, INVISIBLE_WHITE, true})
               .delay(1.25)
               .fadeTo(1, 0.5)
               .beginParallel()
               .scaleTo({1.5, 1.5}, 1)
               .moveTo({1000, 200}, 2, EaseType::EaseOut)
               .fadeTo(0, 2)
               .endParallel()
               .delay(4)
               .moveTo({900, 100}, 0).scaleTo({1.0f, 1.0f}, 0)
               .fadeTo(1, 0.5)
               .beginParallel()
               .scaleTo({1.5, 1.5}, 1)
               .moveTo({1000, 200}, 2, EaseType::EaseOut)
               .fadeTo(0, 2)
               .endParallel();

    Sprite s6_meteor04;
    s6_meteor04.set_id(assetsMap[Asset::Meteor04].id)
               .init({{120, 120}, {300, 0}, {1.25f, 1.25f}, 0, INVISIBLE_WHITE, true})
               .delay(1.5)
               .fadeTo(1, 0.5)
               .beginParallel()
               .scaleTo({1.5, 1.5}, 1)
               .moveTo({500, 200}, 2, EaseType::EaseOut)
               .fadeTo(0, 2)
               .endParallel()
               .delay(4)
               .moveTo({300, 0}, 0).scaleTo({1.25f, 1.25f}, 0)
               .fadeTo(1, 0.5)
               .beginParallel()
               .scaleTo({1.5, 1.5}, 1)
               .moveTo({500, 200}, 2, EaseType::EaseOut)
               .fadeTo(0, 2)
               .endParallel();

    Sprite s6_meteor05;
    s6_meteor05.set_id(assetsMap[Asset::Meteor05].id)
               .init({{100, 100}, {800, 100}, {1.0f, 1.0f}, 0, INVISIBLE_WHITE, true})
               .delay(2)
               .fadeTo(1, 0.5)
               .beginParallel()
               .scaleTo({1.5, 1.5}, 1)
               .moveTo({1000, 300}, 2, EaseType::EaseOut)
               .fadeTo(0, 2)
               .endParallel()
               .delay(4)
               .moveTo({800, 100}, 0).scaleTo({1.0f, 1.0f}, 0)
               .fadeTo(1, 0.5)
               .beginParallel()
               .scaleTo({1.5, 1.5}, 1)
               .moveTo({1000, 300}, 2, EaseType::EaseOut)
               .fadeTo(0, 2)
               .endParallel();

    Sprite s6_meteor06;
    s6_meteor06.set_id(assetsMap[Asset::Meteor06].id)
               .init({{120, 120}, {100, 0}, {1.0f, 1.0f}, 0, INVISIBLE_WHITE, true})
               .delay(2.5)
               .fadeTo(1, 0.5)
               .beginParallel()
               .scaleTo({1.25, 1.25}, 1)
               .moveTo({300, 200}, 2, EaseType::EaseOut)
               .fadeTo(0, 2)
               .endParallel()
               .delay(4)
               .moveTo({100, 0}, 0).scaleTo({1.0f, 1.0f}, 0)
               .fadeTo(1, 0.5)
               .beginParallel()
               .scaleTo({1.5, 1.5}, 1)
               .moveTo({300, 200}, 2, EaseType::EaseOut)
               .fadeTo(0, 2)
               .endParallel();

    Sprite s6_meteor07;
    s6_meteor07.set_id(assetsMap[Asset::Meteor07].id)
               .init({{150, 150}, {1200, 200}, {0.75f, 0.75f}, 0, INVISIBLE_WHITE, true})
               .delay(3)
               .fadeTo(1, 0.5)
               .beginParallel()
               .scaleTo({1.5, 1.5}, 1)
               .moveTo({1400, 500}, 2, EaseType::EaseOut)
               .fadeTo(0, 2)
               .endParallel()
               .delay(4)
               .moveTo({1200, 200}, 0).scaleTo({0.75f, 0.75f}, 0)
               .fadeTo(1, 0.5)
               .beginParallel()
               .scaleTo({1.5, 1.5}, 1)
               .moveTo({1400, 500}, 2, EaseType::EaseOut)
               .fadeTo(0, 2)
               .endParallel();


    Sprite s6_meteor08;
    s6_meteor08.set_id(assetsMap[Asset::Meteor08].id)
               .init({{120, 150}, {1100, 0}, {0.75f, 0.75f}, 0, INVISIBLE_WHITE, true})
               .delay(3.5)
               .fadeTo(1, 0.5)
               .beginParallel()
               .scaleTo({1.5, 1.5}, 1)
               .moveTo({1300, 200}, 2, EaseType::EaseOut)
               .fadeTo(0, 2)
               .endParallel()
               .delay(4)
               .moveTo({1100, 0}, 0).scaleTo({0.75f, 1.25}, 0)
               .fadeTo(1, 0.5)
               .beginParallel()
               .scaleTo({1.5, 1.5}, 1)
               .moveTo({1300, 200}, 2, EaseType::EaseOut)
               .fadeTo(0, 2)
               .endParallel();

    Sprite s6_meteor09;
    s6_meteor09.set_id(assetsMap[Asset::Meteor09].id)
               .init({{100, 100}, {250, 0}, {1.0f, 1.0f}, 0, INVISIBLE_WHITE, true})
               .delay(4)
               .fadeTo(1, 0.5)
               .beginParallel()
               .scaleTo({1.5, 1.5}, 1)
               .moveTo({450, 200}, 2, EaseType::EaseOut)
               .fadeTo(0, 2)
               .endParallel()
               .delay(4)
               .moveTo({250, 0}, 0).scaleTo({1.0f, 1.0f}, 0)
               .fadeTo(1, 0.5)
               .beginParallel()
               .scaleTo({1.5, 1.5}, 1)
               .moveTo({450, 200}, 2, EaseType::EaseOut)
               .fadeTo(0, 2)
               .endParallel();

    Sprite s6_meteor10;
    s6_meteor10.set_id(assetsMap[Asset::Meteor10].id)
               .init({{150, 150}, {700, 0}, {1.0f, 1.0f}, 0, INVISIBLE_WHITE, true})
               .delay(4.5)
               .fadeTo(1, 0.5)
               .beginParallel()
               .scaleTo({1.5, 1.5}, 1)
               .moveTo({900, 200}, 2, EaseType::EaseOut)
               .fadeTo(0, 2)
               .endParallel()
               .delay(4)
               .moveTo({700, 0}, 0).scaleTo({1.0f, 1.0f}, 0)
               .fadeTo(1, 0.5)
               .beginParallel()
               .scaleTo({1.5, 1.5}, 1)
               .moveTo({900, 200}, 2, EaseType::EaseOut)
               .fadeTo(0, 2)
               .endParallel();


    s6.addSprite(s6_meteor01, assetsMap[Asset::Meteor01].drawFunction);
    s6.addSprite(s6_meteor02, assetsMap[Asset::Meteor02].drawFunction);
    s6.addSprite(s6_meteor03, assetsMap[Asset::Meteor03].drawFunction);
    s6.addSprite(s6_meteor04, assetsMap[Asset::Meteor04].drawFunction);
    s6.addSprite(s6_meteor05, assetsMap[Asset::Meteor05].drawFunction);
    s6.addSprite(s6_meteor06, assetsMap[Asset::Meteor06].drawFunction);
    s6.addSprite(s6_meteor07, assetsMap[Asset::Meteor07].drawFunction);
    s6.addSprite(s6_meteor08, assetsMap[Asset::Meteor08].drawFunction);
    s6.addSprite(s6_meteor09, assetsMap[Asset::Meteor09].drawFunction);
    s6.addSprite(s6_meteor10, assetsMap[Asset::Meteor10].drawFunction);
    
    s6.addSprite(s6_ground, assetsMap[Asset::TrainMid].drawFunction);
    s6.addSprite(s6_penguin, assetsMap[Asset::Penguin].drawFunction);

    s6.addSprite(s6_apple, assetsMap[Asset::Apple].drawFunction);
    s6.addSprite(s6_banana, assetsMap[Asset::Banana].drawFunction);
    s6.addSprite(s6_suica, assetsMap[Asset::Suica].drawFunction);

    s6.addSprite(s6_door, assetsMap[Asset::Henshin].drawFunction);

    s6.addSprite(s6_running001, assetsMap[Asset::RunningMan001].drawFunction);
    s6.addSprite(s6_running001_rainbowStroke, assetsMap[Asset::RunningMan003].drawFunction);
    s6.addSprite(s6_running001_rainbowTexture, assetsMap[Asset::RunningMan003].drawFunction);
    s6.addSprite(s6_running003, assetsMap[Asset::RunningMan003].drawFunction);
    s6.addSprite(s6_henshin, assetsMap[Asset::Henshin].drawFunction);

    s6.addSprite(s6_map, assetsMap[Asset::Map].drawFunction);

    // Scene 7
    Scene s7;

    Sprite s7_map;
    s7_map.set_id(assetsMap[Asset::Map].id)
          .init({{600, 480}, {500, 150}, {4.0f, 4.0f}, 0, WHITE})
          .delay(3)
          .scaleTo({0, 0}, 1, EaseType::EaseInOut)
          .delay(15).fadeTo(0, 1);

    Sprite s7_bg;
    s7_bg.set_id(assetsMap[Asset::Background].id)
         .initBackground(BLACK);

    Sprite s7_backgroundIce;
    s7_backgroundIce.set_id(assetsMap[Asset::BackgroundIce].id)
                    .init({{1600, 900}, {0, 0}, {1.0f, 1.0f}, 0, WHITE})
                    .delay(18).fadeTo(0, 1);;


    Sprite s7_running001_rainbowTexture;
    s7_running001_rainbowTexture.set_id(assetsMap[Asset::RunningMan001].id)
                                .init({
                                    {210, 300}, {875, 500}, {0.0f, 0.0f}, DirectX::XMConvertToRadians(-90), WHITE, true,
                                    ShaderType::RainbowTexture
                                })
                                .beginParallel()
                                .rotateTo(DirectX::XMConvertToRadians(-60), 1)
                                .scaleTo({0.75f, 0.75f}, 1)
                                .endParallel()
                                .moveTo({1000, -100}, 1, EaseType::EaseOut)
                                .beginParallel()
                                .moveTo({1100, -100}, 1)
                                .scaleTo({0.0f, 0.0f}, 1)
                                .endParallel();


    Sprite s7_running001_rainbowTexture2;
    s7_running001_rainbowTexture2.set_id(assetsMap[Asset::RunningMan001].id)
                                 .init({
                                     {210, 300}, {1600, 500}, {1.0f, 1.0f}, 0, WHITE, false,
                                     ShaderType::RainbowTexture
                                 })
                                 .delay(4)
                                 .moveTo({700, 500}, 3, EaseType::EaseOut)
                                 .fadeTo(0, 1);

    Sprite s7_running001;
    s7_running001.set_id(assetsMap[Asset::RunningMan001].id)
                 .init({
                     {210, 300}, {700, 500}, {1.0f, 1.0f}, 0, INVISIBLE_WHITE, false
                 })
                 .delay(7.4)
                 .fadeTo(1, 0.6)
                 .delay(0.5)
                 .fadeTo(0, 0.6);


    Sprite s7_running003;
    s7_running003.set_id(assetsMap[Asset::RunningMan003].id)
                 .init({
                     {210, 300}, {700, 500}, {1.0f, 1.0f}, 0, INVISIBLE_WHITE, false
                 })
                 .delay(8.9)
                 .fadeTo(1, 0.6)
                 .delay(0.5)
                 .fadeTo(0, 0.6);

    Sprite s7_running003_SLOW;
    s7_running003_SLOW.set_id(assetsMap[Asset::RunningMan003_SLOW].id)
                      .init({
                          {210, 300}, {700, 500}, {1.0f, 1.0f}, 0, INVISIBLE_WHITE, false
                      })
                      .delay(10)
                      .fadeTo(1, 0.6)
                      .delay(1)
                      .fadeTo(0, 0.6);


    Sprite s7_running003_STOP;
    s7_running003_STOP.set_id(assetsMap[Asset::RunningMan003_STOP].id)
                      .init({
                          {210, 300}, {700, 500}, {1.0f, 1.0f}, 0, INVISIBLE_WHITE, false
                      })
                      .delay(11.5)
                      .fadeTo(1, 0.5)
                      .delay(1)
                      .fadeTo(0, 0.5);

    Sprite s7_ground;
    s7_ground.set_id(assetsMap[Asset::Ground].id)
             .init({{4000.0f, 900.0f}, {-1200.0f, 0.0f}, {1.0f, 1.0f}, 0.0f, WHITE, false}).delay(18).fadeTo(0, 1)
             ;


    Sprite s7_wing;
    s7_wing.set_id(assetsMap[Asset::Wing].id)
           .init({{320.0f, 320.0f}, {640.0f, 500.0f}, {1.0f, 1.0f}, 0.0f, INVISIBLE_WHITE, false})
           .delay(13)
           .fadeTo(1, 0)
           .delay(0.9)
           .fadeTo(0, 0);


    Sprite s7_wrap;
    s7_wrap.set_id(assetsMap[Asset::Wrap].id)
           .init({{320.0f, 320.0f}, {640.0f, 500.0f}, {1.0f, 1.0f}, 0.0f, INVISIBLE_WHITE, false})
           .delay(13)
           .fadeTo(1, 0)
           .delay(0.9)
           .fadeTo(0, 0);

    Sprite s7_fubuki;
    s7_fubuki.set_id(assetsMap[Asset::Fubuki].id)
             .initBackground()
             .fadeTo(0, 0)
             .delay(15)
             .fadeTo(1, 0)
             .delay(2)
             .fadeTo(0, 1);

    s7.addSprite(s7_bg, assetsMap[Asset::Background].drawFunction);
    s7.addSprite(s7_backgroundIce, assetsMap[Asset::BackgroundIce].drawFunction);
    s7.addSprite(s7_ground, assetsMap[Asset::Map].drawFunction);

    s7.addSprite(s7_map, assetsMap[Asset::Map].drawFunction);

    s7.addSprite(s7_running001_rainbowTexture, assetsMap[Asset::RunningMan001].drawFunction);
    s7.addSprite(s7_running001_rainbowTexture2, assetsMap[Asset::RunningMan001].drawFunction);
    s7.addSprite(s7_running001, assetsMap[Asset::RunningMan001].drawFunction);
    s7.addSprite(s7_running003, assetsMap[Asset::RunningMan003].drawFunction);
    s7.addSprite(s7_running003_SLOW, assetsMap[Asset::RunningMan003_SLOW].drawFunction);
    s7.addSprite(s7_running003_STOP, assetsMap[Asset::RunningMan003_STOP].drawFunction);

    s7.addSprite(s7_wing, assetsMap[Asset::Wing].drawFunction);
    s7.addSprite(s7_wrap, assetsMap[Asset::Wrap].drawFunction);
    s7.addSprite(s7_fubuki, assetsMap[Asset::Fubuki].drawFunction);

    // Scene 8
    // Title 1: Kooler Presents
    Sprite s8_bg;
    s8_bg.set_id(assetsMap[Asset::Background].id)
         .initBackground(BLACK);

    std::string s8_title_text = "THE  END\n\n   BY   \n KOOLER\n";
    std::string s8_credit_text = "Assets from pipoya.net/dotown and generated by Adobe Firefly";
    Sprite s8_title;
    s8_title.initCenterTitle(s1_title_text, INVISIBLE_WHITE)
            .delay(1)
            .fadeTo(1, 2, EaseType::EaseInOut)
            .delay(8)
            .fadeTo(0, 2, EaseType::EaseInOut);

    Sprite s8_credit;
    s8_credit
        .init({{0, 0}, {1600.0f, 700.0f}, {1.0f, 1.0f}, 0.0f, WHITE, false})
        .delay(1)
        .moveTo({-1600, 700}, 7)
        .delay(5);

    Scene s8;
    s8.addSprite(s8_bg, assetsMap[Asset::Background].drawFunction);
    s8.addSprite(s8_title, makeTextDrawFunction(s8_title_text, config.titlePresentsText));
    s8.addSprite(s8_credit, makeTextDrawFunction(s8_credit_text, config.titlePresentsText));
    
    // Scene 9
    Sprite s9_stageClose;
    s9_stageClose.set_id(assetsMap[Asset::StageClose].id).initBackground(INVISIBLE_WHITE).fadeTo(1, 0.5);
    
    Scene s9;
    s9.addSprite(s8_bg, assetsMap[Asset::Background].drawFunction);
    s9.addSprite(s9_stageClose, assetsMap[Asset::StageClose].drawFunction);
    
    RegisterScene(s1);
    RegisterScene(s2);
    RegisterScene(s3);
    RegisterScene(s4);
    RegisterScene(s5);
    RegisterScene(s6);
    RegisterScene(s7);
    RegisterScene(s8);
    RegisterScene(s9);
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
