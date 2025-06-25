#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "game_window.h"

#include "direct3d.h"
#include "sprite.h"
#include "shader.h"
#include "texture.h"

#include "DirectXMath.h"
#include "sprite_anim.h"

#include "debug_text.h"
#include <sstream>

#include "system_timer.h"

int APIENTRY WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE,
    _In_ LPSTR,
    _In_ int nCmdShow)
{
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    constexpr int SCREEN_WIDTH = 1600;
    constexpr int SCREEN_HEIGHT = 900;
    HWND hWnd = GameWindow_Create(hInstance, SCREEN_WIDTH, SCREEN_HEIGHT);

    SystemTimer_Initialize();
    Direct3D_Initialize(hWnd, SCREEN_WIDTH, SCREEN_HEIGHT);
    Shader_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());
    Sprite_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());
    Texture_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());
    SpriteAnim_Initialize();

    // デバッグテキスト
    hal::DebugText debugText(
        Direct3D_GetDevice(),
        Direct3D_GetContext(),
        L"assets/consolab_ascii_512.png",
        Direct3D_GetBackBufferWidth(),
        Direct3D_GetBackBufferHeight(),
        0.0f,
        0.0f,
        0,
        0,
        0.0f,
        0.0f
    );

    // テクスチャ読み込み
    int texid_knight_winter = Texture_Load(L"assets/knight_3.png");
    int texid_kokosozai = Texture_Load(L"assets/kokosozai.png");
    int texid_runningman001 = Texture_Load(L"assets/runningman001.png");

    std::vector<int> ids;

    ids.emplace_back(SpriteAnim_CreatePlayer(SpriteAnim_RegisterPattern(texid_kokosozai, 13, 16, 0.1,
                                                                        {32, 32}, {0, 32 * 0})));
    ids.emplace_back(SpriteAnim_CreatePlayer(SpriteAnim_RegisterPattern(texid_kokosozai, 13, 16, 0.5,
                                                                        {32, 32}, {0, 32 * 1})));
    ids.emplace_back(SpriteAnim_CreatePlayer(SpriteAnim_RegisterPattern(texid_kokosozai, 6, 16, 0.2,
                                                                        {32, 32}, {0, 32 * 2})));
    ids.emplace_back(SpriteAnim_CreatePlayer(SpriteAnim_RegisterPattern(texid_kokosozai, 8, 16, 0.2,
                                                                        {32, 32}, {0, 32 * 3})));
    ids.emplace_back(SpriteAnim_CreatePlayer(SpriteAnim_RegisterPattern(texid_kokosozai, 15, 16, 0.2,
                                                                        {32, 32}, {0, 32 * 4})));
    ids.emplace_back(SpriteAnim_CreatePlayer(SpriteAnim_RegisterPattern(texid_kokosozai, 4, 16, 0.2,
                                                                        {32, 32}, {32 * 2, 32 * 5}, false)));

    int patternIdRun = SpriteAnim_RegisterPattern(texid_runningman001, 10, 5, 0.1,
                                                  {140, 200}, {0, 0}, true);
    int playIdRun = SpriteAnim_CreatePlayer(patternIdRun);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // fps・実行フレーム計測用
    double exec_last_time = SystemTimer_GetTime();
    double fps_last_time = exec_last_time;
    double current_time = 0.0;
    ULONG frame_count = 0;
    double fps = 0;

    // ゲームループ＆メッセージループ
    MSG msg;

    do
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            // ウィンドウメッセージが来ていたら 
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            // fps 計測
            current_time = SystemTimer_GetTime(); // システム時刻を取得
            double elapsed_time = current_time - fps_last_time; // fps計測用の経過時間を計算

            if (elapsed_time >= 1.0) // 1秒ごとに計測
            {
                fps = frame_count / elapsed_time;
                fps_last_time = current_time; // FPSを測定した時刻を保存
                frame_count = 0; // カウントをクリア
            }

            elapsed_time = current_time - exec_last_time;
            if (elapsed_time >= (1.0 / 60.0)) // 1/60秒ごとに実行
            // if (true)
            {
                exec_last_time = current_time; // 処理した時刻を保存

                // ゲームの処理 
                Direct3D_Clear();
                Sprite_Begin();

                Sprite_Draw(texid_knight_winter, 0, 0, 128, 128);

                for (int i = 0; i < ids.size(); i++)
                {
                    SpriteAnim_Draw(ids[i], static_cast<float>((32 + 128) * i), 32.0f, 128.0f, 128.0f);
                }
                SpriteAnim_Draw(playIdRun, 0, static_cast<float>(32 + 128 + 32), 140.0f, 200.0f);

                SpriteAnim_Update(elapsed_time);

                // DirectX::XMFLOAT4 color = {1.0f, 1.0f, 1.0f, 1.0f};
                //
                // Sprite_Draw(texid_knight_winter, 32.0f, 32.0f, color);
                // Sprite_Draw(texid_knight_winter, 512.0f, 32.0f, 256, 256, color);
                // Sprite_Draw(texid_kokosozai, 800.0f, 32.0f, 32.0f, 64.0f, 32.0f, 32.0f, color);
                // Sprite_Draw(texid_kokosozai, 864.0f, 32.0f, 32.0f, 32.0f * 2, 32.0f, 32.0f, 256.0f, 256.0f, color);

#if defined(DEBUG) || defined(_DEBUG)
                std::stringstream ss;
                ss << fps;

                debugText.SetText("FPS:");
                debugText.SetText(ss.str().c_str());

                debugText.Draw();
                debugText.Clear();

#endif
                Direct3D_Present();

                frame_count++;
            }
        }
    }
    while (msg.message != WM_QUIT);

    SpriteAnim_Finalize();
    Texture_Finalize();
    Sprite_Finalize();
    Shader_Finalize();
    Direct3D_Finalize();

    return (int)msg.wParam;
}
