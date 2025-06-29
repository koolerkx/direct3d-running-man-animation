/**
* @file SceneSprite.h
 * @brief スプライトの状態管理
 * @details Keyframeシステムに近く、動きを登録して、特定の時間の状態を計算する
 * @author KOOLER FAN
 * @date 2025/06/30
 */

#ifndef SceneSprite_h
#define SceneSprite_h

#include <DirectXMath.h>
#include <string>
#include <vector>

enum class ShaderType: unsigned int
{
    Normal = 0,
    RainbowTexture = 1,
    RainbowStroke = 2,
    RainbowStrokeOnly = 3
};

enum class EaseType
{
    Linear,
    EaseIn,
    EaseOut,
    EaseInOut,
    Bounce,
    Elastic,
    Back
};

enum class AnimProperty
{
    Position,
    Scale,
    Rotation,
    Alpha,
    Color,
    Flip,
    Shader
};

enum class RepeatMode
{
    Normal,
    PingPong
};

struct SpriteState
{
    DirectX::XMFLOAT2 size = {64.0f, 64.0f};

    DirectX::XMFLOAT2 position = {0.0f, 0.0f};
    DirectX::XMFLOAT2 scale = {1.0f, 1.0f};
    float rotation = 0.0f;
    DirectX::XMFLOAT4 color{1.0f, 1.0f, 1.0f, 1.0f};
    bool flipY = false;
    ShaderType shaderType = ShaderType::Normal;
};

struct AnimationKeyframe
{
    AnimProperty property;
    DirectX::XMFLOAT4 targetValue;
    double duration;
    EaseType easing;
    double startTime;

    bool isParallel = false;
    int repeatGroupIndex = -1; ///< @brief -1??J???????

    AnimationKeyframe(AnimProperty prop, DirectX::XMFLOAT4 target, double dur, EaseType ease)
        : property(prop), targetValue(target), duration(dur), easing(ease), startTime(0.0)
    {
    }

    AnimationKeyframe(AnimProperty prop, DirectX::XMFLOAT2 target, double dur, EaseType ease)
        : property(prop), targetValue({target.x, target.y, 0, 0}), duration(dur), easing(ease), startTime(0.0)
    {
    }

    AnimationKeyframe(AnimProperty prop, float target, double dur, EaseType ease)
        : property(prop), targetValue({target, 0, 0, 0}), duration(dur), easing(ease), startTime(0.0)
    {
    }
};

struct RepeatGroup
{
    std::vector<AnimationKeyframe> keyframes;
    double startTime;
    double duration;
    RepeatMode mode;
    int repeatCount;
};

struct ParallelGroup
{
    std::vector<AnimationKeyframe> keyframes;
    double groupStartTime;
    double groupDuration;
};

class SceneSprite
{
public:
    int drawingId;

    SceneSprite();
    SceneSprite(int id);


    // 初期化
    SceneSprite& set_id(int id); ///< @param id texId or play id
    SceneSprite& init(const SpriteState& state);

    // 変換
    SceneSprite& moveTo(DirectX::XMFLOAT2 pos, double duration, EaseType easing = EaseType::Linear);
    SceneSprite& scaleTo(DirectX::XMFLOAT2 scale, double duration, EaseType easing = EaseType::Linear);
    SceneSprite& rotateTo(float rotation, double duration, EaseType easing = EaseType::Linear);
    SceneSprite& fadeTo(float alpha, double duration, EaseType easing = EaseType::Linear);
    SceneSprite& colorTo(DirectX::XMFLOAT4 color, double duration, EaseType easing = EaseType::Linear);

    // 他
    SceneSprite& delay(double duration);
    SceneSprite& flip();
    SceneSprite& ShaderTo(ShaderType shader);

    // 非同期動きグループ
    SceneSprite& beginParallel();
    SceneSprite& endParallel();

    // 繰り返しグループ
    SceneSprite& beginRepeat(RepeatMode mode = RepeatMode::Normal, int times = -1);
    SceneSprite& endRepeat();

    // 出力
    SpriteState getState(double timeOffset);

    const double getDuration();

    // 他
    SceneSprite& initBackground(DirectX::XMFLOAT4 color = {1.0f, 1.0f, 1.0f, 1.0f});
    SceneSprite& initCenterTitle(std::string text, DirectX::XMFLOAT4 color = {1.0f, 1.0f, 1.0f, 1.0f});

private:
    SpriteState initialState;
    SpriteState currentState;
    std::vector<AnimationKeyframe> timeline;
    double totalDuration = 0.0;

    // 非同期動きグループ状態
    bool inParallelGroup = false;
    double parallelGroupStartTime = 0.0;
    std::vector<AnimationKeyframe> currentParallelKeyframes;
    std::vector<ParallelGroup> parallelGroups;

    bool inRepeatGroup = false;
    RepeatGroup currentRepeatGroup{};
    std::vector<RepeatGroup> repeatGroups;

    void addKeyframe(AnimationKeyframe keyframe);

    static float applyEasing(float t, EaseType easing);
    static float interpolate(float start, float end, float t);
    static DirectX::XMFLOAT2 interpolate(DirectX::XMFLOAT2 start, DirectX::XMFLOAT2 end, float t);
    static DirectX::XMFLOAT4 interpolate(DirectX::XMFLOAT4 start, DirectX::XMFLOAT4 end, float t);
};

// 色
using COLOR = DirectX::XMFLOAT4;

// 基本色
constexpr COLOR BLACK = {0.0f, 0.0f, 0.0f, 1.0f}; // 黒
constexpr COLOR WHITE = {1.0f, 1.0f, 1.0f, 1.0f}; // 白
constexpr COLOR INVISIBLE_BLACK = {0.0f, 0.0f, 0.0f, 0.0f}; // 透明黒
constexpr COLOR INVISIBLE_WHITE = {1.0f, 1.0f, 1.0f, 0.0f}; // 透明白

constexpr COLOR DARK_GRAY = {0.1f, 0.1f, 0.1f, 1.0f}; // 濃いグレー（黒の代用におすすめ）
constexpr COLOR ALMOST_BLACK = {0.05f, 0.05f, 0.05f, 1.0f}; // ほぼ黒（完全な黒の代用）
constexpr COLOR INVISIBLE_DARK_GRAY = {0.1f, 0.1f, 0.1f, 1.0f}; // 濃いグレー（黒の代用におすすめ）
constexpr COLOR INVISIBLE_ALMOST_BLACK = {0.05f, 0.05f, 0.05f, 1.0f}; // ほぼ黒（完全な黒の代用）

// 純色
constexpr COLOR RED = {1.0f, 0.0f, 0.0f, 1.0f}; // 赤
constexpr COLOR GREEN = {0.0f, 1.0f, 0.0f, 1.0f}; // 緑
constexpr COLOR BLUE = {0.0f, 0.0f, 1.0f, 1.0f}; // 青
constexpr COLOR YELLOW = {1.0f, 1.0f, 0.0f, 1.0f}; // 黄
constexpr COLOR CYAN = {0.0f, 1.0f, 1.0f, 1.0f}; // シアン
constexpr COLOR MAGENTA = {1.0f, 0.0f, 1.0f, 1.0f}; // マゼンタ

// 明るい色・特殊色
constexpr COLOR NEON = {0.22f, 1.0f, 0.078f, 1.0f}; // 蛍光グリーン
constexpr COLOR AQUA = {0.0f, 1.0f, 1.0f, 1.0f}; // アクア（水色）
constexpr COLOR GOLD = {1.0f, 0.84f, 0.0f, 1.0f}; // 金
constexpr COLOR SILVER = {0.75f, 0.75f, 0.75f, 1.0f}; // 銀

// 淡色・中性色
constexpr COLOR GRAY = {0.5f, 0.5f, 0.5f, 1.0f}; // グレー
constexpr COLOR BROWN = {0.6f, 0.3f, 0.1f, 1.0f}; // ブラウン

// その他のよく使う色
constexpr COLOR ORANGE = {1.0f, 0.5f, 0.0f, 1.0f}; // オレンジ
constexpr COLOR PURPLE = {0.5f, 0.0f, 0.5f, 1.0f}; // 紫
constexpr COLOR PINK = {1.0f, 0.0f, 0.5f, 1.0f}; // ピンク

#endif
