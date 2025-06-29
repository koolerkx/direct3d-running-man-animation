#include "SceneSprite.h"

#include <algorithm>

#include "debug_ostream.h"
#include "debug_text.h"
#include "direct3d.h"
using namespace DirectX;

constexpr int DrawingIDDefaultPlaceholder = -1;

ScreenScprite::ScreenScprite()
{
    drawingId = DrawingIDDefaultPlaceholder;
}

ScreenScprite::ScreenScprite(int id)
{
    drawingId = id;
}

ScreenScprite& ScreenScprite::set_id(int id)
{
    drawingId = id;
    return *this;
}

ScreenScprite& ScreenScprite::init(const SpriteState& state)
{
    initialState = currentState = state;
    timeline.clear();
    totalDuration = 0.0;

    return *this;
}

ScreenScprite& ScreenScprite::moveTo(XMFLOAT2 pos, double duration, EaseType easing)
{
    AnimationKeyframe keyframe(AnimProperty::Position, pos, duration, easing);
    addKeyframe(keyframe);
    return *this;
}

ScreenScprite& ScreenScprite::scaleTo(XMFLOAT2 scale, double duration, EaseType easing)
{
    AnimationKeyframe keyframe(AnimProperty::Scale, scale, duration, easing);
    addKeyframe(keyframe);
    return *this;
}

ScreenScprite& ScreenScprite::rotateTo(float rotation, double duration, EaseType easing)
{
    AnimationKeyframe keyframe(AnimProperty::Rotation, rotation, duration, easing);
    addKeyframe(keyframe);
    return *this;
}

ScreenScprite& ScreenScprite::fadeTo(float alpha, double duration, EaseType easing)
{
    XMFLOAT4 color = currentState.color;
    color.w = alpha;
    AnimationKeyframe keyframe(AnimProperty::Alpha, color, duration, easing);
    addKeyframe(keyframe);
    return *this;
}

ScreenScprite& ScreenScprite::colorTo(XMFLOAT4 color, double duration, EaseType easing)
{
    AnimationKeyframe keyframe(AnimProperty::Color, 0, duration, easing);
    addKeyframe(keyframe);
    return *this;
}

ScreenScprite& ScreenScprite::delay(double duration)
{
    totalDuration += duration;
    return *this;
}

ScreenScprite& ScreenScprite::flip()
{
    AnimationKeyframe keyframe(AnimProperty::Flip, 0, 0, EaseType::Linear);
    addKeyframe(keyframe);
    return *this;
}

ScreenScprite& ScreenScprite::ShaderTo(ShaderType shader)
{
    AnimationKeyframe keyframe(AnimProperty::Shader, static_cast<int>(shader), 0, EaseType::Linear);
    addKeyframe(keyframe);
    return *this;
}

// 非同期動きグループ
ScreenScprite& ScreenScprite::beginParallel()
{
    if (inParallelGroup) return *this;

    inParallelGroup = true;
    parallelGroupStartTime = totalDuration;
    currentParallelKeyframes.clear();

    return *this;
}

ScreenScprite& ScreenScprite::endParallel()
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

// 繰り返しグループ
ScreenScprite& ScreenScprite::beginRepeat(RepeatMode mode, int times)
{
    if (inRepeatGroup) return *this;

    inRepeatGroup = true;
    currentRepeatGroup.keyframes.clear();
    currentRepeatGroup.startTime = totalDuration;
    currentRepeatGroup.duration = 0.0;
    currentRepeatGroup.mode = mode;
    currentRepeatGroup.repeatCount = times;

    return *this;
}

ScreenScprite& ScreenScprite::endRepeat()
{
    if (!inRepeatGroup) return *this;

    inRepeatGroup = false;
    double groupDuration = 0.0;
    for (const AnimationKeyframe& keyframe : currentRepeatGroup.keyframes)
    {
        groupDuration = max(groupDuration, (keyframe.startTime - currentRepeatGroup.startTime) + keyframe.duration);
    }
    currentRepeatGroup.duration = groupDuration;

    repeatGroups.push_back(currentRepeatGroup);

    if (currentRepeatGroup.repeatCount < 0)
    {
        totalDuration = currentRepeatGroup.startTime + currentRepeatGroup.duration;
    }
    else
    {
        totalDuration = currentRepeatGroup.startTime + (groupDuration * currentRepeatGroup.repeatCount);
    }

    currentRepeatGroup.keyframes.clear();

    return *this;
}

SpriteState ScreenScprite::getState(double timeOffset)
{
    if (timeOffset <= 0.0) return initialState;

    SpriteState state = initialState;

    for (const AnimationKeyframe& keyframe : timeline)
    {
        double effectiveTime = timeOffset;

        if (keyframe.repeatGroupIndex >= 0 && keyframe.repeatGroupIndex < repeatGroups.size())
        {
            const RepeatGroup& repeatGroup = repeatGroups[keyframe.repeatGroupIndex];

            double groupTime = timeOffset - repeatGroup.startTime;
            if (groupTime <= 0) continue; // まだ始まっていない

            if (repeatGroup.repeatCount == -1 || groupTime < (repeatGroup.duration * repeatGroup.repeatCount))
            {
                int cycle = static_cast<int>(groupTime / repeatGroup.duration);
                double cycleTime = fmod(groupTime, repeatGroup.duration);

                if (repeatGroup.mode == RepeatMode::PingPong)
                {
                    if (cycle % 2 == 1)
                    {
                        cycleTime = repeatGroup.duration - cycleTime;
                    }
                }

                effectiveTime = repeatGroup.startTime + cycleTime;
            }
            else if (timeOffset > repeatGroup.startTime + (repeatGroup.duration * repeatGroup.repeatCount))
            {
                double finalCycleTime = 0.0;
                if (repeatGroup.mode == RepeatMode::PingPong)
                {
                    finalCycleTime = (repeatGroup.repeatCount % 2 != 0) ? repeatGroup.duration : 0.0;
                }
                effectiveTime = repeatGroup.startTime + finalCycleTime;
            }
        }

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

const double ScreenScprite::getDuration()
{
    return totalDuration;
}

ScreenScprite& ScreenScprite::initBackground(XMFLOAT4 color)
{
    const int SCREEN_WIDTH = static_cast<int>(Direct3D_GetBackBufferWidth());
    const int SCREEN_HEIGHT = static_cast<int>(Direct3D_GetBackBufferHeight());

    return init({
        {static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT)}, {0.0f, 0.0f}, {1.0f, 1.0f},
        0.0f, color
    });
}

ScreenScprite& ScreenScprite::initCenterTitle(std::string text, XMFLOAT4 color)
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

void ScreenScprite::addKeyframe(AnimationKeyframe keyframe)
{
    if (inRepeatGroup)
    {
        keyframe.repeatGroupIndex = static_cast<int>(repeatGroups.size());
        currentRepeatGroup.keyframes.push_back(keyframe);
        timeline.push_back(keyframe);
        totalDuration += keyframe.duration;
    }
    else if (inParallelGroup)
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


float ScreenScprite::applyEasing(float t, EaseType easing)
{
    t = max(0.0f, min(1.0f, t));

    switch (easing)
    {
    case EaseType::Linear:
        return t;

    case EaseType::EaseIn:
        return t * t;

    case EaseType::EaseOut:
        return 1.0f - (1.0f - t) * (1.0f - t);

    case EaseType::EaseInOut:
        return t < 0.5f ? 2.0f * t * t : 1.0f - 2.0f * (1.0f - t) * (1.0f - t);

    case EaseType::Bounce:
        {
            if (t < 1.0f / 2.75f)
                return 7.5625f * t * t;
            else if (t < 2.0f / 2.75f)
            {
                t -= 1.5f / 2.75f;
                return 7.5625f * t * t + 0.75f;
            }
            else if (t < 2.5f / 2.75f)
            {
                t -= 2.25f / 2.75f;
                return 7.5625f * t * t + 0.9375f;
            }
            else
            {
                t -= 2.625f / 2.75f;
                return 7.5625f * t * t + 0.984375f;
            }
        }

    case EaseType::Elastic:
        {
            if (t == 0.0f) return 0.0f;
            if (t == 1.0f) return 1.0f;

            float p = 0.3f;
            float s = p / 4.0f;
            return pow(2.0f, -10.0f * t) * sin((t - s) * (2.0f * 3.14159f) / p) + 1.0f;
        }

    case EaseType::Back:
        {
            const float c1 = 1.70158f;
            const float c3 = c1 + 1.0f;
            return c3 * t * t * t - c1 * t * t;
        }

    default:
        return t;
    }
}

float ScreenScprite::interpolate(float start, float end, float t)
{
    return start + (end - start) * t;
}

XMFLOAT2 ScreenScprite::interpolate(XMFLOAT2 start, XMFLOAT2 end, float t)
{
    return {
        interpolate(start.x, end.x, t),
        interpolate(start.y, end.y, t)
    };
}

XMFLOAT4 ScreenScprite::interpolate(XMFLOAT4 start, XMFLOAT4 end, float t)
{
    return {
        interpolate(start.x, end.x, t),
        interpolate(start.y, end.y, t),
        interpolate(start.z, end.z, t),
        interpolate(start.w, end.w, t)
    };
}
