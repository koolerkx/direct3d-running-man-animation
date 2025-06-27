#pragma once
#include <map>
#include <vector>
#include <xstring>

#include "debug_text.h"
#include "Scene.h"

static std::vector<Scene> scenes;
static int sceneIndex = 0;
static bool isPrevSceneEnd = true;

enum class Texture
{
    Background
};

static std::map<Texture, std::wstring> texturePaths{
    {Texture::Background, L"assets/white.png"}
};

static std::map<Texture, int> textureMap; 

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
