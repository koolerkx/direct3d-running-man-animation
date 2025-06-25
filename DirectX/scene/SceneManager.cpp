#include "SceneManager.h"

#include "system_timer.h"

SceneManager& SceneManager::getInstance() {
    static SceneManager instance;
    return instance;
}

void SceneManager::registerScene (Scene scene)
{
    scenes.emplace_back(scene);
}

void SceneManager::start()
{
    startTime = SystemTimer_GetTime();
}