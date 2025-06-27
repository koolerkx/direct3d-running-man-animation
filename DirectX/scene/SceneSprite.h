#pragma once
#include <DirectXMath.h>
#include <vector>

enum class EaseType
{
    Linear
};

enum class AnimProperty
{
    Position,
    Scale,
    Rotation,
    Alpha,
    Color
};

enum class LoopType
{
    None
};

enum class RepeatMode
{
    Normal
};

struct SpriteState
{
    DirectX::XMFLOAT2 position = {0.0f, 0.0f};
    DirectX::XMFLOAT2 scale = {1.0f, 1.0f};
    float rotation = 0.0f;
    float alpha = 1.0f;
    DirectX::XMFLOAT4 color{0.0f, 0.0f, 0.0f, 1.0f};
};

struct AnimationKeyframe
{
    AnimProperty property;
    DirectX::XMFLOAT4 targetValue;
    double duration;
    EaseType easing;
    double startTime;

    AnimationKeyframe(AnimProperty prop, DirectX::XMFLOAT4 target, double dur, EaseType ease)
        : property(prop), targetValue(target), duration(dur), easing(ease), startTime(0.0) {}

    AnimationKeyframe(AnimProperty prop, DirectX::XMFLOAT2 target, double dur, EaseType ease)
        : property(prop), targetValue({target.x, target.y, 0, 0}), duration(dur), easing(ease), startTime(0.0) {}

    AnimationKeyframe(AnimProperty prop, float target, double dur, EaseType ease)
        : property(prop), targetValue({target, 0, 0, 0}), duration(dur), easing(ease), startTime(0.0) {}
};

class Sprite
{
public:
    Sprite() = default; // FIXME: Not expected to call
    Sprite(const wchar_t* texturePath);
    
    // 初期化
    Sprite* init(const SpriteState& state);

    // 変換
    Sprite* moveTo(DirectX::XMFLOAT2 pos, double duration, EaseType easing = EaseType::Linear);
    Sprite* scaleTo(DirectX::XMFLOAT2 scale, double duration, EaseType easing = EaseType::Linear);
    Sprite* rotateTo(float rotation, double duration, EaseType easing = EaseType::Linear);
    Sprite* fadeTo(float alpha, double duration, EaseType easing = EaseType::Linear);
    Sprite* colorTo(DirectX::XMFLOAT4 color, double duration, EaseType easing = EaseType::Linear);

    // 他
    Sprite* delay(double duration);

    // 出力
    SpriteState getState(double timeOffset);
    void draw(double timeOffset);

private:
    int textureId = -1;
    
    SpriteState initialState;
    SpriteState currentState;
    std::vector<AnimationKeyframe> timeline;
    double totalDuration = 0.0;

    void addKeyframe(AnimationKeyframe keyframe);

    static float interpolate(float start, float end, float t);
    static DirectX::XMFLOAT2 interpolate(DirectX::XMFLOAT2 start, DirectX::XMFLOAT2 end, float t);
    static DirectX::XMFLOAT4 interpolate(DirectX::XMFLOAT4 start, DirectX::XMFLOAT4 end, float t);
};
