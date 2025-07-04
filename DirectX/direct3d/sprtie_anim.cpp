/**
* @file sprite_anim.cpp
 * @brief スプライトアニメーション描画
 * @author KOOLER FAN
 * @date 2025/06/17
 */
#include "sprite_anim.h"
#include "sprite.h"
#include <DirectXMath.h>

#include "debug_helper.h"
#include "debug_ostream.h"

using namespace DirectX;

struct AnimPlayData
{
    int m_PatternId{-1}; // アニメーションパターンID
    int m_PatternNum{0}; // 現在再生中のパターン番号
    double m_accumulated_time{0}; // 累積時間
};

static constexpr int ANIM_PATTERN_MAX = 128;
static AnimPatternData g_AnimPattern[ANIM_PATTERN_MAX];
static constexpr int ANIM_PLAY_MAX = 256;
static AnimPlayData g_AnimPlay[ANIM_PLAY_MAX];

void SpriteAnim_Initialize()
{
    // アニメーションパターン管理情報を初期化（すべて利用していない）状況にする
    for (AnimPatternData& data : g_AnimPattern)
    {
        data.m_TextureId = -1;
    }

    for (AnimPlayData& data : g_AnimPlay)
    {
        data.m_PatternId = -1;
    }
}

void SpriteAnim_Finalize(void)
{
}

void SpriteAnim_Update(double elapsed_time)
{
    for (int i = 0; i < ANIM_PLAY_MAX; i++)
    {
        if (g_AnimPlay[i].m_PatternId < 0) continue; // データが入ってない

        const int pattern_id = g_AnimPlay[i].m_PatternId;
        AnimPatternData* pPatternData = &g_AnimPattern[pattern_id];

        if (g_AnimPlay[i].m_accumulated_time >= pPatternData->m_seconds_per_pattern)
        {
            g_AnimPlay[i].m_PatternNum++;

            if (g_AnimPlay[i].m_PatternNum >= pPatternData->m_PatternMax)
            {
                if (pPatternData->m_IsLooped)
                {
                    g_AnimPlay[i].m_PatternNum = 0;
                }
                else
                {
                    g_AnimPlay[i].m_PatternNum = pPatternData->m_PatternMax - 1;
                }
            }

            g_AnimPlay[i].m_accumulated_time -= pPatternData->m_seconds_per_pattern;
        }
        g_AnimPlay[i].m_accumulated_time += elapsed_time;
    }
}

void SpriteAnim_Draw(int playid, float dx, float dy, float dw, float dh)
{
    const int pattern_id = g_AnimPlay[playid].m_PatternId;
    const int pattern_num = g_AnimPlay[playid].m_PatternNum;
    AnimPatternData* pPatternData = &g_AnimPattern[pattern_id];

    Sprite_Draw(
        pPatternData->m_TextureId,
        dx, dy,
        static_cast<float>(pPatternData->m_StartPosition.x + pPatternData->m_PatternSize.x * (pattern_num % pPatternData
            ->m_HPatternMax)),
        static_cast<float>(pPatternData->m_StartPosition.y + pPatternData->m_PatternSize.y * (pattern_num / pPatternData
            ->m_HPatternMax)),
        static_cast<float>(pPatternData->m_PatternSize.x),
        static_cast<float>(pPatternData->m_PatternSize.y),
        dw, dh
    );
}

void SpriteAnim_Draw(int playid, SpriteState spriteState)
{
    const int pattern_id = g_AnimPlay[playid].m_PatternId;
    const int pattern_num = g_AnimPlay[playid].m_PatternNum;
    AnimPatternData* pPatternData = &g_AnimPattern[pattern_id];

    Sprite_Draw(
        pPatternData->m_TextureId,
        spriteState,
        static_cast<float>(pPatternData->m_StartPosition.x + pPatternData->m_PatternSize.x * (pattern_num % pPatternData
            ->m_HPatternMax)),
        static_cast<float>(pPatternData->m_StartPosition.y + pPatternData->m_PatternSize.y * (pattern_num / pPatternData
            ->m_HPatternMax)),
        static_cast<float>(pPatternData->m_PatternSize.x),
        static_cast<float>(pPatternData->m_PatternSize.y)
    );
}

int SpriteAnim_RegisterPattern(int textureId, int patternMax, int m_HPatternMax, double m_seconds_per_pattern,
                               DirectX::XMUINT2 patternSize,
                               DirectX::XMUINT2 patternStartPosition, bool isLoop)
{
    for (int i = 0; i < ANIM_PATTERN_MAX; i++)
    {
        if (g_AnimPattern[i].m_TextureId >= 0) continue;

        g_AnimPattern[i].m_TextureId = textureId;
        g_AnimPattern[i].m_PatternMax = patternMax;
        g_AnimPattern[i].m_HPatternMax = m_HPatternMax;
        g_AnimPattern[i].m_seconds_per_pattern = m_seconds_per_pattern;
        g_AnimPattern[i].m_PatternSize = patternSize;
        g_AnimPattern[i].m_StartPosition = patternStartPosition;
        g_AnimPattern[i].m_IsLooped = isLoop;

        return i;
    }

    return -1;
}

int SpriteAnim_RegisterPattern(int textureId, AnimPatternData patternData)
{
    return SpriteAnim_RegisterPattern(
        textureId,
        patternData.m_PatternMax,
        patternData.m_HPatternMax,
        patternData.m_seconds_per_pattern,
        patternData.m_PatternSize,
        patternData.m_StartPosition,
        patternData.m_IsLooped
    );
}

int SpriteAnim_CreatePlayer(int anim_pattern_id)
{
    for (int i = 0; i < ANIM_PLAY_MAX; i++)
    {
        if (g_AnimPlay[i].m_PatternId >= 0) continue;

        g_AnimPlay[i].m_PatternId = anim_pattern_id;
        g_AnimPlay[i].m_accumulated_time = 0;
        g_AnimPlay[i].m_PatternNum = 0;

        return i;
    }
    return -1;
}
