#pragma once
#include <vector>

#include "Scene.h"

class SceneManager
{
public:
    static SceneManager& getInstance();
    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;

    void registerScene(Scene scene);

    void start();

    void drawFrame();
    
private:
    SceneManager() = default;
    std::vector<Scene> scenes;

    double startTime;
};
