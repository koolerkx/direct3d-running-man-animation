#include "SceneSprite.h"

#include <algorithm>

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
        if (keyframe.startTime > timeOffset) continue;

        double progress = min(1.0, (timeOffset - keyframe.startTime) / keyframe.duration);

        switch (keyframe.property)
        {
        case AnimProperty::Position:
            {
                XMFLOAT2 start = (keyframe.startTime == 0.0) ? initialState.position : state.position;
                XMFLOAT2 target = {keyframe.targetValue.x, keyframe.targetValue.y};
                state.position = interpolate(start, target, progress);
                break;
            }
        case AnimProperty::Scale:
            {
                XMFLOAT2 start = (keyframe.startTime == 0.0) ? initialState.scale : state.scale;
                XMFLOAT2 target = {keyframe.targetValue.x, keyframe.targetValue.y};
                state.scale = interpolate(start, target, progress);
                break;
            }
        case AnimProperty::Rotation:
            {
                float start = (keyframe.startTime == 0.0) ? initialState.rotation : state.rotation;
                state.rotation = interpolate(start, keyframe.targetValue.x, progress);
                break;
            }
        case AnimProperty::Alpha:
            {
                float start = (keyframe.startTime == 0.0) ? initialState.alpha : state.alpha;
                state.color.w = interpolate(start, keyframe.targetValue.x, progress);
                break;
            }
        case AnimProperty::Color:
            {
                XMFLOAT4 start = (keyframe.startTime == 0.0) ? initialState.color : state.color;
                state.color = interpolate(start, keyframe.targetValue, progress);
                break;
            }
        }
    }

    return state;
}

void Sprite::draw(double timeOffset)
{
    SpriteState state = getState(timeOffset);

    constexpr float width = 128.0f;
    constexpr float height = 128.0f;

    Sprite_Draw(textureId,
                state.position.x, state.position.y,
                width * state.scale.x, height * state.scale.y, XMConvertToRadians(state.rotation), state.color);
}

void Sprite::addKeyframe(AnimationKeyframe keyframe)
{
    keyframe.startTime = totalDuration;
    timeline.push_back(keyframe);
    totalDuration += keyframe.duration;
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
