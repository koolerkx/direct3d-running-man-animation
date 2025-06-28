/**
 * @file sprite_anim.h
 * @brief スプライトアニメーション描画
 * @author KOOLER FAN
 * @date 2025/06/17
 */

#ifndef SPRITE_ANIM_H
#define SPRITE_ANIM_H

#include <DirectXMath.h>

#include "SceneSprite.h"


struct AnimPatternData
{
    int m_TextureId{1}; ///> テクスチャID @retval -1 登録されていない 
    int m_PatternMax{0}; // パターン数
    int m_HPatternMax{0}; // 一列（横）のパターン最大数
    double m_seconds_per_pattern = 0.1;
    DirectX::XMUINT2 m_PatternSize{0, 0}; // 1パターンサイズ
    DirectX::XMUINT2 m_StartPosition{0, 0}; // アニメーションのスタート座標
    bool m_IsLooped{false}; // ループするか
};

void SpriteAnim_Initialize();
void SpriteAnim_Finalize(void);

void SpriteAnim_Update(double elapsed_time);
void SpriteAnim_Draw(int playid, float dx, float dy, float dw, float dh);

void SpriteAnim_Draw(int playid, SpriteState spriteState);

/**
 * 
 * @param textureId 管理番号
 * @param patternMax パターンの画像数
 * @param m_HPatternMax 一列（横）のパターン最大数
 * @param m_seconds_per_pattern 再生速度、１フレームかかる時間
 * @param patternSize パターン一個のサイズ
 * @param patternStartPosition 最初のパターンの左上座標
 * @param isLoop ループするか
 * @return
 *
 * @pre 必ずTexture_Load()のあと
 */
int SpriteAnim_RegisterPattern(
    int textureId,
    int patternMax,
    int m_HPatternMax,
    double m_seconds_per_pattern,
    DirectX::XMUINT2 patternSize,
    DirectX::XMUINT2 patternStartPosition,
    bool isLoop = true
);

int SpriteAnim_RegisterPattern(
    int textureId,
    AnimPatternData patternData);

int SpriteAnim_CreatePlayer(int anim_pattern_id);

#endif
