[English](README.md) | [日本語](README.ja.md)

# DirectX 11 Scene Management & Fluent Keyframes

> **Homework**: DirectX 11 sprite animation lesson using a keyframe-driven fluent interface.

## Project Overview

This assignment demonstrates:
- Declarative **scene definitions** using keyframes
- Automatic **scene sequencing** and playback
- A **fluent keyframe API** for transformations & effects

It builds upon the sample code in [koolerkx/learn-directX](https://github.com/koolerkx/learn-directX).

## Architecture
1. **SceneManager**
   - Files: [SceneManager.h](DirectX/scene/SceneManager.h), [SceneManager.cpp](DirectX/scene/SceneManager.cpp)
   - Responsibilities:
     - **SceneInitialize(config)**: load textures & sprite patterns into `assetsMap`
     - **SceneDefinition(config)**: define scenes via fluent API, then `RegisterScene(scene)`
     - **SceneLoop()**: per-frame update + draw; auto-advance when a scene ends

2. **Scene**
   - Files: [Scene.h](DirectX/scene/Scene.h), [Scene.cpp](DirectX/scene/Scene.cpp)
   - Responsibilities:
     - Maintain a list of `{ SceneSprite, drawFunction }`
     - **startSetup()**: compute scene’s total duration, record start time
     - **draw_loop()**: call each sprite’s drawFunction with its current `SpriteState`; flag `is_end` when done

3. **SceneSprite**
   - Files: [SceneSprite.h](DirectX/scene/SceneSprite.h), [SceneSprite.cpp](DirectX/scene/SceneSprite.cpp)
   - Encapsulates a single drawable’s timeline of **AnimationKeyframe** entries
   - **Keyframe Internals**:
     ```cpp
     struct AnimationKeyframe {
         AnimProperty property;        // e.g. Position, Scale, Alpha, Color, Flip, Shader
         DirectX::XMFLOAT4 target;      // target value
         double duration, startTime;
         EaseType easing;               // easing function
         bool isParallel;               // part of parallel group
     };
     std::vector<AnimationKeyframe> timeline;
     double totalDuration;
     void addKeyframe(AnimationKeyframe kf) { /* assign startTime, push, update totalDuration */ }
     ```
   - **getState(elapsedTime)**: iterate timeline, apply easing/interpolation, return `SpriteState`
   - **Fluent API**:
     - **Init**: `set_id()`, `initBackground()`, `initCenterTitle()`
     - **Transforms**: `moveTo()`, `scaleTo()`, `rotateTo()`, `fadeTo()`, `colorTo()`
     - **Controls**: `delay()`, `flip()`, `ShaderTo()`
     - **Parallel**: `beginParallel()` … `endParallel()`

## Predefined Colors

[SceneSprite.h](DirectX/scene/SceneSprite.h) defines the alias `COLOR` (for `DirectX::XMFLOAT4`) and several `constexpr` color constants:

- **BLACK**: (0.0, 0.0, 0.0, 1.0)
- **WHITE**: (1.0, 1.0, 1.0, 1.0)
- **INVISIBLE_BLACK**: (0.0, 0.0, 0.0, 0.0)
- **INVISIBLE_WHITE**: (1.0, 1.0, 1.0, 0.0)

Primary colors:

- **RED**, **GREEN**, **BLUE**, **YELLOW**, **CYAN**, **MAGENTA**

Extended colors:

- **NEON**, **AQUA**, **GOLD**, **SILVER**, **ORANGE**, **PURPLE**, **PINK**

## Rainbow Shaders

SceneSprite supports switching pixel shaders at runtime via `ShaderTo(ShaderType)`. The available rainbow shaders live under `DirectX/shaders/`:

- [shader_pixel_2d_rainbow_stroke.hlsl](DirectX/shaders/shader_pixel_2d_rainbow_stroke.hlsl) ? `ShaderType::RainbowStroke`
- [shader_pixel_2d_rainbow_stroke_only.hlsl](DirectX/shaders/shader_pixel_2d_rainbow_stroke_only.hlsl) ? `ShaderType::RainbowStrokeOnly`
- [shader_pixel_2d_rainbow_tex.hlsl](DirectX/shaders/shader_pixel_2d_rainbow_tex.hlsl) ? `ShaderType::RainbowTexture`

## Fluent Interface Example
*(Fluent methods declared in [SceneSprite.h](DirectX/scene/SceneSprite.h); scene registration in [SceneManager.cpp](DirectX/scene/SceneManager.cpp).)*

```cpp
// Define a simple fade‐in/out scene
SceneSprite background;
background.set_id(assetsMap[Asset::Background].id)
          .initBackground(WHITE)
          .fadeTo(0.0f, 1.0)    // fade out to transparent over 1s
          .delay(0.5)           // wait 0.5s
          .fadeTo(1.0f, 1.0);   // fade in to opaque over 1s

Scene s;
s.addSprite(background, assetsMap[Asset::Background].drawFunction);
RegisterScene(s);
```

- **Parallel keyframes** example:

```cpp
// Move and scale at the same time
sprite.beginParallel()
      .moveTo({800, 600}, 2.0, EaseType::EaseOut)
      .scaleTo({2.0f, 2.0f}, 2.0, EaseType::EaseIn)
      .endParallel();
```

- **Combined keyframes** example:

```cpp
// Chain multiple properties with various easing functions
SceneSprite sprite;
sprite.set_id(assetsMap[Asset::RunningMan003].id)
      .init({{210, 300}, {500, 500}, {1.0f, 1.0f}, 0.0f, WHITE})
      .moveTo({800, 500}, 1.0, EaseType::EaseIn)                            // ease-in move
      .rotateTo(DirectX::XMConvertToRadians(180), 1.5, EaseType::EaseOut)    // ease-out rotate
      .scaleTo({1.5f, 1.5f}, 2.0, EaseType::Bounce)                         // bounce scale
      .colorTo(CYAN, 0.8, EaseType::Back)                                   // back-easing color change
      .fadeTo(0.0f, 0.5, EaseType::Linear)                                  // linear fade out
      .flip();                                                              // vertical flip

Scene s2;
s2.addSprite(sprite, assetsMap[Asset::RunningMan003].drawFunction);
RegisterScene(s2);
```

## Scene Loop

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

Each frame, `draw_loop()` updates and renders all sprites for the current scene. When its total duration passes, the manager automatically progresses to the next scene.