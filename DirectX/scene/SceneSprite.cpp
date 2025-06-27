#include "SceneSprite.h"

#include <algorithm>

#include "debug_ostream.h"
#include "sprite.h"
#include "texture.h"
using namespace DirectX;

Sprite::Sprite(const wchar_t* texturePath)
{
    textureId = Texture_Load(texturePath);
}

Sprite* Sprite::init(const SpriteState& state)
{
    initialState = currentState = state;
    timeline.clear();
    totalDuration = 0.0;

    return this;
}

Sprite* Sprite::moveTo(XMFLOAT2 pos, double duration, EaseType easing)
{
    AnimationKeyframe keyframe(AnimProperty::Position, pos, duration, easing);
    addKeyframe(keyframe);
    return this;
}

Sprite* Sprite::scaleTo(XMFLOAT2 scale, double duration, EaseType easing)
{
    AnimationKeyframe keyframe(AnimProperty::Scale, scale, duration, easing);
    addKeyframe(keyframe);
    return this;
}

Sprite* Sprite::rotateTo(float rotation, double duration, EaseType easing)
{
    AnimationKeyframe keyframe(AnimProperty::Rotation, rotation, duration, easing);
    addKeyframe(keyframe);
    return this;
}

Sprite* Sprite::fadeTo(float alpha, double duration, EaseType easing)
{
    AnimationKeyframe keyframe(AnimProperty::Alpha, alpha, duration, easing);
    addKeyframe(keyframe);
    return this;
}

Sprite* Sprite::colorTo(XMFLOAT4 color, double duration, EaseType easing)
{
    AnimationKeyframe keyframe(AnimProperty::Color, color, duration, easing);
    addKeyframe(keyframe);
    return this;
}

Sprite* Sprite::beginRepeat(RepeatMode mode, int times)
{
    if (inRepeatGroup) return this;

    inRepeatGroup = true;
    currentRepeatGroup.keyframes.clear();
    currentRepeatGroup.startTime = totalDuration;
    currentRepeatGroup.duration = 0.0;
    currentRepeatGroup.mode = mode;
    currentRepeatGroup.repeatCount = times;

    return this;
}

Sprite* Sprite::endRepeat()
{
    if (!inRepeatGroup) return this;

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

    return this;
}

Sprite* Sprite::delay(double duration)
{
    totalDuration += duration;
    return this;
}

SpriteState Sprite::getState(double timeOffset)
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
                float start = (keyframe.startTime == 0.0) ? initialState.alpha : state.alpha;
                state.color.w = interpolate(start, keyframe.targetValue.x, easedProgress);
                break;
            }
        case AnimProperty::Color:
            {
                XMFLOAT4 start = (keyframe.startTime == 0.0) ? initialState.color : state.color;
                state.color = interpolate(start, keyframe.targetValue, easedProgress);
                break;
            }
        }
    }

    return state;
}

void Sprite::draw(double timeOffset)
{
    SpriteState state = getState(timeOffset);

    constexpr float width = 64.0f;
    constexpr float height = 64.0f;

    Sprite_Draw(textureId,
                state.position.x, state.position.y,
                width * state.scale.x, height * state.scale.y,
                XMConvertToRadians(state.rotation),
                state.color
    );
}

void Sprite::addKeyframe(AnimationKeyframe keyframe)
{
    keyframe.startTime = totalDuration;

    if (inRepeatGroup)
    {
        keyframe.repeatGroupIndex = static_cast<int>(repeatGroups.size());
        currentRepeatGroup.keyframes.push_back(keyframe);
    }

    timeline.push_back(keyframe);
    totalDuration += keyframe.duration;
}


float Sprite::applyEasing(float t, EaseType easing)
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

float Sprite::interpolate(float start, float end, float t)
{
    return start + (end - start) * t;
}

XMFLOAT2 Sprite::interpolate(XMFLOAT2 start, XMFLOAT2 end, float t)
{
    return {
        interpolate(start.x, end.x, t),
        interpolate(start.y, end.y, t)
    };
}

XMFLOAT4 Sprite::interpolate(XMFLOAT4 start, XMFLOAT4 end, float t)
{
    return {
        interpolate(start.x, end.x, t),
        interpolate(start.y, end.y, t),
        interpolate(start.z, end.z, t),
        interpolate(start.w, end.w, t)
    };
}
