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
    BackgroundStar,

    StageOpen,
    StageClose,

    ForegroundJapan,
    BackgroundJapan,
    BackgroundWorldEnd,
    Ground,

    ForegroundShinjyuku,
    BackgroundShinjyuku01,
    BackgroundShinjyuku02
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
                -1, 10, 5, 0.1, {140, 200}, {0, 0}, true
            }
        }
    },
    {
        Asset::BackgroundStar, AnimPatternDataWithPath{
            L"assets/starburst_640x480-min.png", {
                -1, 5, 2, 0.05, {640, 480}, {0, 0}, true
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
                -1, 18, 1, 0.05, {640, 480}, {0, 0}, true
            }
        }
    },
    {Asset::ForegroundJapan, L"assets/S1_FG1.png"},
    {Asset::BackgroundJapan, L"assets/S1_BB1.png"},
    {Asset::BackgroundWorldEnd, L"assets/S1_BB2.png"},
    {Asset::Ground, L"assets/S1_Ground.png"},
    {Asset::ForegroundShinjyuku, L"assets/S2_FG.png"},
    {Asset::BackgroundShinjyuku01, L"assets/S2_BG1.png"},
    {Asset::BackgroundShinjyuku02, L"assets/S2_BG2.png"},
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
