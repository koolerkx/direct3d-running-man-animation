#pragma once
#include <map>
#include <variant>
#include <vector>
#include <xstring>

#include "debug_text.h"
#include "Scene.h"
#include "sprite_anim.h"

static std::vector<Scene> scenes;
static int sceneIndex = 0;
static bool isPrevSceneEnd = true;

struct AnimPatternDataWithPath
{
    std::wstring path;
    AnimPatternData patternData;
};

enum class Asset
{
    Background,
    RunningMan000,
    RunningMan001,
    RunningMan003,
    RunningMan003_SLOW,
    RunningMan003_STOP,
    BackgroundStar,

    StageOpen,
    StageClose,

    ForegroundJapan,
    BackgroundJapan,
    BackgroundWorldEnd,
    Ground,

    ForegroundShinjyuku,
    BackgroundShinjyuku01,
    BackgroundShinjyuku02,

    BackgroundIce,

    Enemy1,
    Skill1,
    Explosion1,
    Explosion2,
    Door,

    UFO,
    TrainFirst,
    TrainMid,
    TrainDoor,

    Penguin,
    Apple,
    Banana,
    Suica,
    Henshin,

    Map,

    Meteor01,
    Meteor02,
    Meteor03,
    Meteor04,
    Meteor05,
    Meteor06,
    Meteor07,
    Meteor08,
    Meteor09,
    Meteor10,

    Wing,
    Wrap,
    Fubuki,
    LaserBackground
};

using AssetDataType = std::variant<std::wstring, AnimPatternDataWithPath>;

static std::map<Asset, AssetDataType> AssetsSetting
{
    {Asset::Background, L"assets/white.png"},
    {
        Asset::RunningMan000, AnimPatternDataWithPath{
            L"assets/runningman000.png", {
                -1, 8, 8, 0.1, {100, 200}, {0, 0}, true
            }
        }
    },
    {
        Asset::RunningMan001, AnimPatternDataWithPath{
            L"assets/runningman001.png", {
                -1, 10, 5, 0.1, {140, 200}, {0, 0}, true
            }
        }
    },
    {
        Asset::RunningMan003, AnimPatternDataWithPath{
            L"assets/runningman003.png", {
                -1, 10, 5, 0.08, {140, 200}, {0, 0}, true
            }
        }
    },
    {
        Asset::RunningMan003_SLOW, AnimPatternDataWithPath{
            L"assets/runningman003.png", {
                -1, 10, 5, 0.2, {140, 200}, {0, 0}, true
            }
        }
    },
    {
        Asset::RunningMan003_STOP, AnimPatternDataWithPath{
            L"assets/runningman003.png", {
                -1, 1, 1, 0.08, {140, 200}, {2 * 140, 1 * 200}, true
            }
        }
    },
    {
        Asset::BackgroundStar, AnimPatternDataWithPath{
            L"assets/starburst_640x480-min.png", {
                -1, 10, 2, 0.03, {640, 480}, {0, 0}, true
            }
        }
    },
    {
        Asset::StageOpen, AnimPatternDataWithPath{
            L"assets/pipo-curtain1.png", {
                -1, 10, 1, 0.1, {640, 480}, {0, 0}, false
            }
        }
    },
    {
        Asset::StageClose, AnimPatternDataWithPath{
            L"assets/pipo-curtain2.png", {
                -1, 18, 1, 0.1, {640, 480}, {0, 0}, false
            }
        }
    },
    {
        Asset::Enemy1, AnimPatternDataWithPath{
            L"assets/pipo-nazoobj05a.png", {
                -1, 6, 5, 0.08, {192, 192}, {0, 0}, true
            }
        }
    },
    {
        Asset::Skill1, AnimPatternDataWithPath{
            L"assets/pipo-btleffect141.png", {
                -1, 30, 3, 0.1, {640, 480}, {0, 0}, true
            }
        }
    },
    {
        Asset::Explosion1, AnimPatternDataWithPath{
            L"assets/pipo-mapeffect005.png", {
                -1, 10, 10, 0.1, {360, 360}, {0, 0}, true
            }
        }
    },
    {
        Asset::Explosion2, AnimPatternDataWithPath{
            L"assets/pipo-btleffect126.png", {
                -1, 16, 2, 0.06, {640, 480}, {0, 0}, true
            }
        }
    },
    {
        Asset::Door, AnimPatternDataWithPath{
            L"assets/pipo-mapeffect016_320.png", {
                -1, 20, 4, 0.05, {320, 240}, {0, 0}, true
            }
        }
    },
    {
        Asset::Henshin, AnimPatternDataWithPath{
            L"assets/pipo-mapeffect009.png", {
                -1, 10, 10, 0.1, {480, 480}, {0, 0}, true
            }
        }
    },
    {
        Asset::Meteor01, AnimPatternDataWithPath{
            L"assets/pipo-mapeffect011a.png", {
                -1, 20, 5, 0.1, {240, 240}, {0, 0}, true
            }
        }
    },
    {
        Asset::Meteor02, AnimPatternDataWithPath{
            L"assets/pipo-mapeffect011b.png", {
                -1, 20, 5, 0.1, {240, 240}, {0, 0}, true
            }
        }
    },
    {
        Asset::Meteor03, AnimPatternDataWithPath{
            L"assets/pipo-mapeffect011c.png", {
                -1, 20, 5, 0.1, {240, 240}, {0, 0}, true
            }
        }
    },
    {
        Asset::Meteor04, AnimPatternDataWithPath{
            L"assets/pipo-mapeffect011d.png", {
                -1, 20, 5, 0.1, {240, 240}, {0, 0}, true
            }
        }
    },
    {
        Asset::Meteor05, AnimPatternDataWithPath{
            L"assets/pipo-mapeffect011e.png", {
                -1, 20, 5, 0.1, {240, 240}, {0, 0}, true
            }
        }
    },
    {
        Asset::Meteor06, AnimPatternDataWithPath{
            L"assets/pipo-mapeffect011f.png", {
                -1, 20, 5, 0.1, {240, 240}, {0, 0}, true
            }
        }
    },
    {
        Asset::Meteor07, AnimPatternDataWithPath{
            L"assets/pipo-mapeffect011g.png", {
                -1, 20, 5, 0.1, {240, 240}, {0, 0}, true
            }
        }
    },
    {
        Asset::Meteor08, AnimPatternDataWithPath{
            L"assets/pipo-mapeffect011h.png", {
                -1, 20, 5, 0.1, {240, 240}, {0, 0}, true
            }
        }
    },
    {
        Asset::Meteor09, AnimPatternDataWithPath{
            L"assets/pipo-mapeffect011i.png", {
                -1, 20, 5, 0.1, {240, 240}, {0, 0}, true
            }
        }
    },
    {
        Asset::Meteor10, AnimPatternDataWithPath{
            L"assets/pipo-mapeffect011j.png", {
                -1, 20, 5, 0.1, {240, 240}, {0, 0}, true
            }
        }
    },
    {
        Asset::Wing, AnimPatternDataWithPath{
            L"assets/wing_640.png", {
                -1, 26, 5, 0.05, {640, 640}, {0, 0}, true
            }
        }
    },
    {
        Asset::Wrap, AnimPatternDataWithPath{
            L"assets/warp_640.png", {
                -1, 26, 2, 0.05, {640, 480}, {0, 0}, true
            }
        }
    },
    {
        Asset::Fubuki, AnimPatternDataWithPath{
            L"assets/kamifubuki64.png", {
                -1, 12, 3, 0.05, {640, 480}, {0, 0}, true
            }
        }
    },
    {
        Asset::LaserBackground, AnimPatternDataWithPath{
            L"assets/pipo-btleffect149a.png", {
                -1, 5, 1, 0.05, {640, 480}, {0, 0}, true
            }
        }
    },

    {Asset::UFO, L"assets/UFO.png"},
    {Asset::TrainFirst, L"assets/train_first.png"},
    {Asset::TrainMid, L"assets/train_mid.png"},
    {Asset::TrainDoor, L"assets/door.png"},

    {Asset::Penguin, L"assets/penguin.png"},
    {Asset::Apple, L"assets/apple.png"},
    {Asset::Banana, L"assets/banana.png"},
    {Asset::Suica, L"assets/suica.png"},

    // Background
    {Asset::ForegroundJapan, L"assets/S1_FG1.png"},
    {Asset::BackgroundJapan, L"assets/S1_BB1.png"},
    {Asset::BackgroundWorldEnd, L"assets/S1_BB2.png"},
    {Asset::Ground, L"assets/S1_Ground.png"},
    {Asset::ForegroundShinjyuku, L"assets/S2_FG.png"},
    {Asset::BackgroundShinjyuku01, L"assets/S2_BG1.png"},
    {Asset::BackgroundShinjyuku02, L"assets/S2_BG2.png"},
    {Asset::BackgroundIce, L"assets/pipo-battlebg005.png"},
    {Asset::Map, L"assets/map.png"},
};

struct AssetsDrawData
{
    int id;
    std::function<void(int, SpriteState)> drawFunction;
};

static std::map<Asset, AssetsDrawData> assetsMap;

struct SceneManagerConfig
{
    hal::DebugText* titlePresentsText;
    hal::DebugText* runningmanText;
};

/**
 * @brief シーンを定義する
 */
void SceneInitialize(SceneManagerConfig config);

void SceneDefinition(SceneManagerConfig config);

/**
 * @brief シーンを追加する
 */
void RegisterScene(Scene scene);

void SceneLoop();

DirectX::XMFLOAT2 GetScreenCenterPosition(const DirectX::XMFLOAT2& size, float screenWidth, float screenHeight);
