/**
 * @file direct3d.cpp
 * @brief Direct3Dの初期化関連
 * @author KOOLER FAN
 * @date 2025/06/06
 */

#include "direct3d.h"
#include <dxgi1_2.h>

#include <dcomp.h>
#pragma comment( lib, "dcomp" )


#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

#include "debug_ostream.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

ComPtr<IDXGIDevice> dxgiDevice;

#if defined(DEBUG) || defined(_DEBUG)
#pragma comment(lib, "DirectXTex_Debug.lib")
#else
#pragma comment(lib, "DirectXTex_Release.lib")
#endif

/* 各種インターフェース */
static ComPtr<ID3D11Device> g_pDevice = nullptr;
static ComPtr<ID3D11DeviceContext> g_pDeviceContext = nullptr;
static ComPtr<IDXGISwapChain1> g_pSwapChain = nullptr;
static ComPtr<ID3D11BlendState> g_pBlendStateMultiply = nullptr;
static ComPtr<ID3D11DepthStencilState> g_pDepthStencilStateDepthDisable = nullptr;

/* バックバッファ関連 */
static ComPtr<ID3D11RenderTargetView> g_pRenderTargetView = nullptr;
static ComPtr<ID3D11Texture2D> g_pDepthStencilBuffer = nullptr;
static ComPtr<ID3D11DepthStencilView> g_pDepthStencilView = nullptr;
static D3D11_TEXTURE2D_DESC g_BackBufferDesc{};

static D3D11_VIEWPORT g_Viewport{};

ComPtr<IDCompositionDevice> g_dcompDevice;
ComPtr<IDCompositionTarget> g_dcompTarget;
ComPtr<IDCompositionVisual> g_dcompVisual;

static bool configureBackBuffer(); // バックバッファの設定・生成


bool Direct3D_Initialize(HWND hWnd, int width, int height)
{
    /* デバイス、スワップチェーン、コンテキスト生成 */
    DXGI_SWAP_CHAIN_DESC1 swap_chain_desc{};
    // swap_chain_desc.Windowed = TRUE; // full screen
    swap_chain_desc.BufferCount = 2; // 裏画面が何個用意する
    swap_chain_desc.Width = width;
    swap_chain_desc.Height = height;
    swap_chain_desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // 色のformat
    swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 何に使う、ここは絵を各場所で使う
    swap_chain_desc.SampleDesc.Count = 1;
    swap_chain_desc.SampleDesc.Quality = 0;
    swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    // swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; // 0: 垂直同期なし
    swap_chain_desc.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;

    /*
    IDXGIFactory1* pFactory;
    CreateDXGIFactory1(IID_PPV_ARGS(&pFactory));
    IDXGIAdapter1* pAdapter;
    pFactory->EnumAdapters1(1, &pAdapter); // セカンダリアダプタを取得
    pFactory->Release();
    DXGI_ADAPTER_DESC1 desc;
    pAdapter->GetDesc1(&desc); // アダプタの情報を取得して確認したい場合
    pAdapter->Release(); // D3D11CreateDeviceAndSwapChain()の第１引数に渡して利用し終わったら解放する
    */

    UINT device_flags = 0;

#if defined(DEBUG) || defined(_DEBUG)
    device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL levels[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0
    };

    D3D_FEATURE_LEVEL feature_level = D3D_FEATURE_LEVEL_11_0;

    HRESULT hr = D3D11CreateDevice(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        device_flags,
        levels,
        ARRAYSIZE(levels),
        D3D11_SDK_VERSION,
        g_pDevice.GetAddressOf(), // 大事
        &feature_level,
        g_pDeviceContext.GetAddressOf() // 大事
    );

    if (SUCCEEDED(hr))
    {
        hr = g_pDevice.As(&dxgiDevice);
    }
    else
    {
        MessageBox(hWnd, "Direct3Dの初期化に失敗しました", "エラー", MB_OK);
        return false;
    }

    ComPtr<IDXGIFactory2> dxgiFactory;
    hr = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));

    hr = dxgiFactory->CreateSwapChainForComposition(
        g_pDevice.Get(),
        &swap_chain_desc,
        nullptr,
        g_pSwapChain.GetAddressOf()
    );

    if (FAILED(hr))
    {
        MessageBox(hWnd, "SwapChainの初期化に失敗しました", "エラー", MB_OK);
        return false;
    }

    hr = DCompositionCreateDevice(
        dxgiDevice.Get(),
        __uuidof(IDCompositionDevice),
        reinterpret_cast<void**>(g_dcompDevice.GetAddressOf())
    );

    hr = g_dcompDevice->CreateTargetForHwnd(hWnd, TRUE, &g_dcompTarget);

    hr = g_dcompDevice->CreateVisual(&g_dcompVisual);
    hr = g_dcompVisual->SetContent(g_pSwapChain.Get());
    hr = g_dcompTarget->SetRoot(g_dcompVisual.Get());
    hr = g_dcompDevice->Commit();

    if (!configureBackBuffer())
    {
        MessageBox(hWnd, "バックバッファの設定に失敗しました", "エラー", MB_OK);
        return false;
    }

    // RGB A -> 好きに使っていい値、基本は透明の表現に使う
    // αテスト、αブレンド
    // ブレンドステート設定
    D3D11_BLEND_DESC bd = {};
    bd.AlphaToCoverageEnable = FALSE;
    bd.IndependentBlendEnable = FALSE;
    bd.RenderTarget[0].BlendEnable = TRUE; // αブレンドするしない

    // src ... ソース（今から描く絵（色）） dest　...　すでに絵描かれた絵（色）

    // RGB
    bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD; // 演算子
    // SrcRGB * SrcBlend + DestRGB * (1 - DestBlend)

    // A
    bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
    bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
    bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    // SrcBlendAlpha * 1 + DestBlendAlpha * 0

    bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    g_pDevice->CreateBlendState(&bd, g_pBlendStateMultiply.GetAddressOf());

    // HACK: 3Dの場合、関数化のほうがいい
    float blend_factor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    g_pDeviceContext->OMSetBlendState(g_pBlendStateMultiply.Get(), blend_factor, 0xffffffff);

    // 深度ステンシルステート設定
    D3D11_DEPTH_STENCIL_DESC dsd = {};
    dsd.DepthFunc = D3D11_COMPARISON_LESS;
    dsd.StencilEnable = FALSE;
    dsd.DepthEnable = FALSE; // 無効にする
    dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;

    g_pDevice->CreateDepthStencilState(&dsd, g_pDepthStencilStateDepthDisable.GetAddressOf());

    // dsd.DepthEnable = TRUE;
    // dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    // g_pDevice->CreateDepthStencilState(&dsd, &g_pDepthStencilStateDepthEnable);

    g_pDeviceContext->OMSetDepthStencilState(g_pDepthStencilStateDepthDisable.Get(), NULL);


    return true;
}

void Direct3D_Finalize()
{
}

void Direct3D_Clear()
{
    float clear_color[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    g_pDeviceContext->ClearRenderTargetView(g_pRenderTargetView.Get(), clear_color);
    g_pDeviceContext->ClearDepthStencilView(g_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

    // レンダーターゲットビューとデプスステンシルビューの設定 
    g_pDeviceContext->OMSetRenderTargets(1, g_pRenderTargetView.GetAddressOf(), g_pDepthStencilView.Get());
}

void Direct3D_Present()
{
    // スワップチェーンの表示
    // 貯まった描画コマンドをグラフィックに転送
    // 0: 垂直同期なし
    g_pSwapChain->Present(1, 0);
}

unsigned int Direct3D_GetBackBufferWidth()
{
    return g_BackBufferDesc.Width;
}

unsigned int Direct3D_GetBackBufferHeight()
{
    return g_BackBufferDesc.Height;
}

ID3D11Device* Direct3D_GetDevice()
{
    return g_pDevice.Get();
}

ID3D11DeviceContext* Direct3D_GetContext()
{
    return g_pDeviceContext.Get();
}


bool configureBackBuffer()
{
    HRESULT hr;

    ID3D11Texture2D* back_buffer_pointer = nullptr;

    // バックバッファの取得
    hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&back_buffer_pointer);

    if (FAILED(hr))
    {
        hal::dout << "バックバッファの取得に失敗しました" << std::endl;
        return false;
    }

    // バックバッファのレンダーターゲットビューの生成
    hr = g_pDevice->CreateRenderTargetView(back_buffer_pointer, nullptr, g_pRenderTargetView.GetAddressOf());

    if (FAILED(hr))
    {
        back_buffer_pointer->Release();
        hal::dout << "バックバッファのレンダーターゲットビューの生成に失敗しました" << std::endl;
        return false;
    }

    // バックバッファの状態（情報）を取得
    back_buffer_pointer->GetDesc(&g_BackBufferDesc);

    back_buffer_pointer->Release(); // バックバッファのポインタは不要なので解放

    // デプスステンシルバッファの生成
    D3D11_TEXTURE2D_DESC depth_stencil_desc{};
    depth_stencil_desc.Width = g_BackBufferDesc.Width;
    depth_stencil_desc.Height = g_BackBufferDesc.Height;
    depth_stencil_desc.MipLevels = 1;
    depth_stencil_desc.ArraySize = 1;
    depth_stencil_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depth_stencil_desc.SampleDesc.Count = 1;
    depth_stencil_desc.SampleDesc.Quality = 0;
    depth_stencil_desc.Usage = D3D11_USAGE_DEFAULT;
    depth_stencil_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depth_stencil_desc.CPUAccessFlags = 0;
    depth_stencil_desc.MiscFlags = 0;
    hr = g_pDevice->CreateTexture2D(&depth_stencil_desc, nullptr, g_pDepthStencilBuffer.GetAddressOf());

    if (FAILED(hr))
    {
        hal::dout << "デプスステンシルバッファの生成に失敗しました" << std::endl;
        return false;
    }

    // デプスステンシルビューの生成
    D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc{};
    depth_stencil_view_desc.Format = depth_stencil_desc.Format;
    depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depth_stencil_view_desc.Texture2D.MipSlice = 0;
    depth_stencil_view_desc.Flags = 0;
    hr = g_pDevice->CreateDepthStencilView(g_pDepthStencilBuffer.Get(), &depth_stencil_view_desc, g_pDepthStencilView.GetAddressOf());

    if (FAILED(hr))
    {
        hal::dout << "デプスステンシルビューの生成に失敗しました" << std::endl;
        return false;
    }

    // ビューポートの設定 
    g_Viewport.TopLeftX = 0.0f;
    g_Viewport.TopLeftY = 0.0f;
    g_Viewport.Width = static_cast<FLOAT>(g_BackBufferDesc.Width);
    g_Viewport.Height = static_cast<FLOAT>(g_BackBufferDesc.Height);
    g_Viewport.MinDepth = 0.0f;
    g_Viewport.MaxDepth = 1.0f;
    g_pDeviceContext->RSSetViewports(1, &g_Viewport); // ビューポートの設定


    return true;
}