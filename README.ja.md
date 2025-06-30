[English](README.md) | [日本語](README.ja.md)

# DirectX 11 Scene Management & Fluent Keyframes

> **課題**: キーフレーム駆動の Fluent インターフェースによる DirectX 11 スプライト アニメーション レッスン。

## プロジェクト概要

* 宣言的な **シーン定義**（キーフレーム使用）
* 自動 **シーンシーケンス** と再生
* 変換＆エフェクト用 **Fluent Keyframe API**

GitHub サンプル: [koolerkx/learn-directX](https://github.com/koolerkx/learn-directX) を基に構築。

## アーキテクチャ

1. **SceneManager**

   * ファイル: `SceneManager.h`, `SceneManager.cpp`
   * 責任:

     * **SceneInitialize(config)**: テクスチャ＆スプライトパターンを `assetsMap` にロード
     * **SceneDefinition(config)**: Fluent API でシーン定義後、`RegisterScene(scene)`
     * **SceneLoop()**: フレーム毎の更新＋描画。シーン終了で自動進行

2. **Scene**

   * ファイル: `Scene.h`, `Scene.cpp`
   * 責任:

     * `{ SceneSprite, drawFunction }` のリスト管理
     * **startSetup()**: シーン全体の総時間計算＆開始時刻記録
     * **draw\_loop()**: 各スプライトの `drawFunction` 実行。終了時に `is_end` フラグ設定

3. **SceneSprite**

   * ファイル: `SceneSprite.h`, `SceneSprite.cpp`
   * 1 つの描画オブジェクトの **AnimationKeyframe** タイムラインをカプセル化
   * **Keyframe 内部構造**:

     ```cpp
     struct AnimationKeyframe {
         AnimProperty property;        // Position, Scale, Alpha, Color, Flip, Shader
         DirectX::XMFLOAT4 target;      // 目標値
         double duration, startTime;
         EaseType easing;               // イージング関数
         bool isParallel;               // 並列グループの一部
     };
     std::vector<AnimationKeyframe> timeline;
     double totalDuration;
     void addKeyframe(AnimationKeyframe kf) { /* startTime 設定, push, totalDuration 更新 */ }
     ```
   * **getState(elapsedTime)**: タイムラインを走査しイージング／補間適用して `SpriteState` を返す
   * **Fluent API**:

     * **初期化**: `set_id()`, `initBackground()`, `initCenterTitle()`
     * **変換**: `moveTo()`, `scaleTo()`, `rotateTo()`, `fadeTo()`, `colorTo()`
     * **制御**: `delay()`, `flip()`, `ShaderTo()`
     * **並列実行**: `beginParallel()` … `endParallel()`

## 事前定義カラー

`SceneSprite.h` で `DirectX::XMFLOAT4` のエイリアス `COLOR` と以下の `constexpr` 色を定義:

* **BLACK**: (0.0, 0.0, 0.0, 1.0)
* **WHITE**: (1.0, 1.0, 1.0, 1.0)
* **INVISIBLE\_BLACK**: (0.0, 0.0, 0.0, 0.0)
* **INVISIBLE\_WHITE**: (1.0, 1.0, 1.0, 0.0)

主要色:

* **RED**, **GREEN**, **BLUE**, **YELLOW**, **CYAN**, **MAGENTA**

拡張色:

* **NEON**, **AQUA**, **GOLD**, **SILVER**, **ORANGE**, **PURPLE**, **PINK**

## レインボーシェーダー

`ShaderTo(ShaderType)` でランタイムにピクセルシェーダー切替可能。レインボーシェーダーは `DirectX/shaders/` に配置:

* `shader_pixel_2d_rainbow_stroke.hlsl` ? `ShaderType::RainbowStroke`
* `shader_pixel_2d_rainbow_stroke_only.hlsl` ? `ShaderType::RainbowStrokeOnly`
* `shader_pixel_2d_rainbow_tex.hlsl` ? `ShaderType::RainbowTexture`

## 流れるようなインタフェース Fluent Interface

*(メソッド定義: `SceneSprite.h`, シーン登録: `SceneManager.cpp`)*

```cpp
// シンプルなフェードイン／アウトシーン定義
SceneSprite background;
background.set_id(assetsMap[Asset::Background].id)
          .initBackground(WHITE)
          .fadeTo(0.0f, 1.0)    // 1 秒で透明にフェードアウト
          .delay(0.5)           // 0.5 秒待機
          .fadeTo(1.0f, 1.0);   // 1 秒で不透明にフェードイン

Scene s;
s.addSprite(background, assetsMap[Asset::Background].drawFunction);
RegisterScene(s);
```

* **並列/同期キーフレーム**例:

```cpp
// 同時に移動と拡大
sprite.beginParallel()
      .moveTo({800, 600}, 2.0, EaseType::EaseOut)
      .scaleTo({2.0f, 2.0f}, 2.0, EaseType::EaseIn)
      .endParallel();
```

* **複合/非同期キーフレーム**例:

```cpp
// 複数プロパティをチェーン、多様なイージング
SceneSprite sprite;
sprite.set_id(assetsMap[Asset::RunningMan003].id)
      .init({{210, 300}, {500, 500}, {1.0f, 1.0f}, 0.0f, WHITE})
      .moveTo({800, 500}, 1.0, EaseType::EaseIn)                            // イーズイン移動
      .rotateTo(DirectX::XMConvertToRadians(180), 1.5, EaseType::EaseOut)    // イーズアウト回転
      .scaleTo({1.5f, 1.5f}, 2.0, EaseType::Bounce)                         // バウンス拡大
      .colorTo(CYAN, 0.8, EaseType::Back)                                   // バックイージングで色変更
      .fadeTo(0.0f, 0.5, EaseType::Linear)                                  // 線形フェードアウト
      .flip();                                                              // 垂直反転

Scene s2;
s2.addSprite(sprite, assetsMap[Asset::RunningMan003].drawFunction);
RegisterScene(s2);
```

## シーンループ

```cpp
void SceneLoop() {
  if (isPrevSceneEnd) {
    scenes[sceneIndex].startSetup();
    isPrevSceneEnd = false;
  }
  scenes[sceneIndex].draw_loop();
  if (scenes[sceneIndex].is_end) {
    sceneIndex++;
    isPrevSceneEnd = true;
  }
  sceneIndex %= scenes.size();
}
```

* 各フレームで `draw_loop()` が現在シーンの全スプライトを更新＆描画
* シーンの総時間経過で自動的に次のシーンへ遷移
