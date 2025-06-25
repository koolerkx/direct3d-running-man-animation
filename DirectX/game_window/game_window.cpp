#include "game_window.h"
#include <algorithm>

// ウィンドウ情報
static constexpr char WINDOW_CLASS[] = "GameWindow"; // メインウインドウクラス名
static constexpr char TITLE[] = "Game"; // 	タイトルバーのテクスト
const constexpr int SCREEN_WIDTH = 1600;
const constexpr int SCREEN_HEIGHT = 900;

// ウインドウプロシージャ　プロトタイプ宣言
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

HWND GameWindow_Create(_In_ HINSTANCE hInstance)
{
    // ウインドウクラスの登録
    WNDCLASSEX wcex{};

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = WINDOW_CLASS;
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    RegisterClassEx(&wcex);

    // メインウィンドウの作成

    RECT window_rect{
        0, 0, SCREEN_WIDTH, SCREEN_HEIGHT
    };

    DWORD style = WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_THICKFRAME);

    AdjustWindowRect(&window_rect, style, FALSE);

    const int WINDOW_WIDTH = window_rect.right - window_rect.left;
    const int WINDOW_HEIGHT = window_rect.bottom - window_rect.top;

    // デスクトップのサイズを取得
    // ライブラリモニターの画面解像度取得
    int desktop_width = GetSystemMetrics(SM_CXSCREEN);
    int desktop_height = GetSystemMetrics(SM_CYSCREEN);

    // ウインドウの表示位置をデスクトップの真ん中に調整する
    // offset to center, use std::max to prevent window overflow
    const int WINDOW_X = std::max(0, (desktop_width - WINDOW_WIDTH) / 2);
    const int WINDOW_Y = std::max(0, (desktop_height - WINDOW_HEIGHT) / 2);

    HWND hWnd = CreateWindow(
        WINDOW_CLASS,
        TITLE,
        style, //　Window Style Flag
        WINDOW_X, // CW_USEDEFAULT
        WINDOW_Y,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    return hWnd;
}

// ウインドウプロシージャ
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_KEYDOWN:
        // https://learn.microsoft.com/ja-jp/windows/win32/inputdev/virtual-key-codes
        if (wParam == VK_ESCAPE)
        {
            SendMessage(hWnd, WM_CLOSE, 0, 0);
        }
        break;
    case WM_CLOSE:
        if (MessageBox(hWnd, "本当に終了してよろしいですか？", "確認", MB_YESNO | MB_DEFBUTTON2) == IDYES)
        {
            DestroyWindow(hWnd);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
