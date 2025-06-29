/**
 * @file SceneSprite.cpp
 * @brief スプライトの状態管理
 * @details Keyframeシステムに近く、動きを登録して、特定の時間の状態を計算する
 * @author KOOLER FAN
 * @date 2025/06/30
 */

#include "SceneSprite.h"

#include <algorithm>

#include "debug_ostream.h"
#include "debug_text.h"
#include "direct3d.h"
using namespace DirectX;

constexpr int DrawingIDDefaultPlaceholder = -1;

SceneSprite::SceneSprite()
{
    drawingId = DrawingIDDefaultPlaceholder;
}

SceneSprite::SceneSprite(int id)
{
    drawingId = id;
}

SceneSprite& SceneSprite::set_id(int id)
{
    drawingId = id;
    return *this;
}

SceneSprite& SceneSprite::init(const SpriteState& state)
{
    initialState = currentState = state;
    timeline.clear();
    totalDuration = 0.0;

    return *this;
}

SceneSprite& SceneSprite::moveTo(XMFLOAT2 pos, double duration, EaseType easing)
{
    AnimationKeyframe keyframe(AnimProperty::Position, pos, duration, easing);
    addKeyframe(keyframe);
    return *this;
}

SceneSprite& SceneSprite::scaleTo(XMFLOAT2 scale, double duration, EaseType easing)
{
    AnimationKeyframe keyframe(AnimProperty::Scale, scale, duration, easing);
    addKeyframe(keyframe);
    return *this;
}

SceneSprite& SceneSprite::rotateTo(float rotation, double duration, EaseType easing)
{
    AnimationKeyframe keyframe(AnimProperty::Rotation, rotation, duration, easing);
    addKeyframe(keyframe);
    return *this;
}

SceneSprite& SceneSprite::fadeTo(float alpha, double duration, EaseType easing)
{
    XMFLOAT4 color = currentState.color;
    color.w = alpha;
    AnimationKeyframe keyframe(AnimProperty::Alpha, color, duration, easing);
    addKeyframe(keyframe);
    return *this;
}

SceneSprite& SceneSprite::colorTo(XMFLOAT4 color, double duration, EaseType easing)
{
    AnimationKeyframe keyframe(AnimProperty::Color, color, duration, easing);
    addKeyframe(keyframe);
    return *this;
}

SceneSprite& SceneSprite::delay(double duration)
{
    totalDuration += duration;
    return *this;
}

SceneSprite& SceneSprite::flip()
{
    AnimationKeyframe keyframe(AnimProperty::Flip, 0, 0, EaseType::Linear);
    addKeyframe(keyframe);
    return *this;
}

SceneSprite& SceneSprite::ShaderTo(ShaderType shader)
{
    AnimationKeyframe keyframe(AnimProperty::Shader, static_cast<float>(shader), 0, EaseType::Linear);
    addKeyframe(keyframe);
    return *this;
}

SceneSprite& SceneSprite::beginParallel()
{
    if (inParallelGroup) return *this;

    inParallelGroup = true;
    parallelGroupStartTime = totalDuration;
    currentParallelKeyframes.clear();

    return *this;
}

SceneSprite& SceneSprite::endParallel()
{
    if (inParallelGroup)
    {
        inParallelGroup = false;

        double maxDuration = 0.0;
        for (AnimationKeyframe& keyframe : currentParallelKeyframes)
        {
            keyframe.startTime = parallelGroupStartTime;
            keyframe.isParallel = true;
            timeline.push_back(keyframe);
            maxDuration = max(maxDuration, keyframe.duration);
        }

        totalDuration = max(totalDuration, parallelGroupStartTime + maxDuration);

        parallelGroups.push_back({
            currentParallelKeyframes,
            parallelGroupStartTime,
            maxDuration,
        });
        currentParallelKeyframes.clear();
    }
    return *this;
}

SpriteState SceneSprite::getState(double timeOffset)
{
    if (timeOffset <= 0.0) return initialState;

    SpriteState state = initialState;

    for (const AnimationKeyframe& keyframe : timeline)
    {
        double effectiveTime = timeOffset;

        if (keyframe.startTime > effectiveTime) continue;

        double progress = min(1.0, (effectiveTime - keyframe.startTime) / keyframe.duration);
        float easedProgress = applyEasing(static_cast<float>(progress), keyframe.easing);

        switch (keyframe.property)
        {
        case AnimProperty::Position:
            {
                XMFLOAT2 start = (keyframe.startTime == 0.0) ? initialState.position : state.position;
                XMFLOAT2 target = {keyframe.targetValue.x, keyframe.targetValue.y};
                state.position = interpolate(start, target, easedProgress);
                break;
            }
        case AnimProperty::Scale:
            {
                XMFLOAT2 start = (keyframe.startTime == 0.0) ? initialState.scale : state.scale;
                XMFLOAT2 target = {keyframe.targetValue.x, keyframe.targetValue.y};
                state.scale = interpolate(start, target, easedProgress);
                break;
            }
        case AnimProperty::Rotation:
            {
                float start = (keyframe.startTime == 0.0) ? initialState.rotation : state.rotation;
                state.rotation = interpolate(start, keyframe.targetValue.x, easedProgress);
                break;
            }
        case AnimProperty::Alpha:
            {
                float start = (keyframe.startTime == 0.0) ? initialState.color.w : state.color.w;
                state.color.w = interpolate(start, keyframe.targetValue.w, easedProgress);
                break;
            }
        case AnimProperty::Color:
            {
                XMFLOAT4 start = (keyframe.startTime == 0.0) ? initialState.color : state.color;
                state.color = interpolate(start, keyframe.targetValue, easedProgress);
                break;
            }
        case AnimProperty::Flip:
            {
                state.flipY = !state.flipY;
                break;
            }
        case AnimProperty::Shader:
            {
                state.shaderType = static_cast<ShaderType>(keyframe.targetValue.x);
                break;
            }
        }
    }

    return state;
}

const double SceneSprite::getDuration()
{
    return totalDuration;
}

SceneSprite& SceneSprite::initBackground(XMFLOAT4 color)
{
    const int SCREEN_WIDTH = static_cast<int>(Direct3D_GetBackBufferWidth());
    const int SCREEN_HEIGHT = static_cast<int>(Direct3D_GetBackBufferHeight());

    return init({
        {static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT)}, {0.0f, 0.0f}, {1.0f, 1.0f},
        0.0f, color
    });
}

SceneSprite& SceneSprite::initCenterTitle(std::string text, XMFLOAT4 color)
{
    const int SCREEN_WIDTH = static_cast<int>(Direct3D_GetBackBufferWidth());
    const int SCREEN_HEIGHT = static_cast<int>(Direct3D_GetBackBufferHeight());
    float title_width = hal::calcCharacterWidth(text);
    float title_height = hal::calcCharacterHeight(text);

    hal::dout << text << " " << title_width << " " << title_height << std::endl;

    return init({
        {0.0f, 0.0f}, {SCREEN_WIDTH / 2.0f - title_width / 2, SCREEN_HEIGHT / 2.0f - title_height / 2},
        {1.0f, 1.0f},
        0.0f, color
    });
}

void SceneSprite::addKeyframe(AnimationKeyframe keyframe)
{
    if (inParallelGroup)
    {
        keyframe.startTime = parallelGroupStartTime;
        keyframe.isParallel = true;
        currentParallelKeyframes.push_back(keyframe);
    }
    else
    {
        keyframe.startTime = totalDuration;
        timeline.push_back(keyframe);
        totalDuration += keyframe.duration;
    }
}


float SceneSprite::applyEasing(float time, EaseType easing)
{
    time = max(0.0f, min(1.0f, time));

    switch (easing)
    {
    case EaseType::Linear:
        return time;

    case EaseType::EaseIn:
        return time * time;

    case EaseType::EaseOut:
        return 1.0f - (1.0f - time) * (1.0f - time);

    case EaseType::EaseInOut:
        return time < 0.5f ? 2.0f * time * time : 1.0f - 2.0f * (1.0f - time) * (1.0f - time);

    case EaseType::Bounce:
        {
            if (time < 1.0f / 2.75f)
                return 7.5625f * time * time;
            else if (time < 2.0f / 2.75f)
            {
                time -= 1.5f / 2.75f;
                return 7.5625f * time * time + 0.75f;
            }
            else if (time < 2.5f / 2.75f)
            {
                time -= 2.25f / 2.75f;
                return 7.5625f * time * time + 0.9375f;
            }
            else
            {
                time -= 2.625f / 2.75f;
                return 7.5625f * time * time + 0.984375f;
            }
        }

    case EaseType::Elastic:
        {
            if (time == 0.0f) return 0.0f;
            if (time == 1.0f) return 1.0f;

            float p = 0.3f;
            float s = p / 4.0f;
            return pow(2.0f, -10.0f * time) * sin((time - s) * (2.0f * 3.14159f) / p) + 1.0f;
        }

    case EaseType::Back:
        {
            const float c1 = 1.70158f;
            const float c3 = c1 + 1.0f;
            return c3 * time * time * time - c1 * time * time;
        }

    default:
        return time;
    }
}

float SceneSprite::interpolate(float start, float end, float time)
{
    return start + (end - start) * time;
}

XMFLOAT2 SceneSprite::interpolate(XMFLOAT2 start, XMFLOAT2 end, float time)
{
    return {
        interpolate(start.x, end.x, time),
        interpolate(start.y, end.y, time)
    };
}

XMFLOAT4 SceneSprite::interpolate(XMFLOAT4 start, XMFLOAT4 end, float t)
{
    return {
        interpolate(start.x, end.x, t),
        interpolate(start.y, end.y, t),
        interpolate(start.z, end.z, t),
        interpolate(start.w, end.w, t)
    };
}
