/**
 * @file Scene.h
 * @brief シーンクラス
 * @details スプライトをまとめて、管理するクラス
 * @author KOOLER FAN
 * @date 2025/06/30
 */

#ifndef SCENE_H
#define SCENE_H

#include <functional>
#include <vector>

#include "SceneSprite.h"

struct SpriteWithDrawFunction
{
    SceneSprite sprite;
    std::function<void(int, SpriteState)>drawFunction;
};

class Scene
{
public:
    bool is_end = false;

    void init();

    void startSetup();
    
    /**
    * @brief シーンが進行しているときの描画関数
    */
    void draw_loop();

    /**
     * @brief シーンに描画するスプライトの追加
     * @param sprite 追加したいスプライト
     * @param drawFunction 描画関数
     */
    void addSprite(SceneSprite sprite, std::function<void(int, SpriteState)> drawFunction);
private:
    double startTime = 0;
    double timeOffset = 0;
    double duration = -1;

    std::vector<SpriteWithDrawFunction> sprites;
};

#endif