/**
 * @file sprite.h
 * @brief スプライト表示
 * @author KOOLER FAN
 * @date 2025/06/12
 */

#include "sprite.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include "DirectXTex.h"
using namespace DirectX;
#include "direct3d.h"
#include "shader.h"
#include "debug_ostream.h"
#include "texture.h"

static constexpr int NUM_VERTEX = 4; // 頂点数

static ID3D11Buffer* g_pVertexBuffer = nullptr; // 頂点バッファ
static ID3D11ShaderResourceView* g_pTexture = nullptr; // テクスチャバッファ

// 注意！初期化で外部から設定されるもの。Release不要。
static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;

float SCREEN_WIDTH;
float SCREEN_HEIGHT;

// 頂点構造体
struct Vertex
{
    XMFLOAT3 position; // 頂点座標
    XMFLOAT4 color; // 色
    XMFLOAT2 uv; // テクスチャー
};

void Sprite_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    // デバイスとデバイスコンテキストのチェック
    if (!pDevice || !pContext)
    {
        hal::dout << "Sprite_Initialize() : 与えられたデバイスかコンテキストが不正です" << std::endl;
        return;
    }

    // デバイスとデバイスコンテキストの保存
    g_pDevice = pDevice;
    g_pContext = pContext;

    // 頂点バッファ生成
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = sizeof(Vertex) * NUM_VERTEX;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    g_pDevice->CreateBuffer(&bd, NULL, &g_pVertexBuffer);
}

void Sprite_Finalize(void)
{
    SAFE_RELEASE(g_pTexture);
    SAFE_RELEASE(g_pVertexBuffer);
}

void Sprite_Begin()
{
    // 頂点情報を書き込み
    SCREEN_WIDTH = static_cast<float>(Direct3D_GetBackBufferWidth());
    SCREEN_HEIGHT = static_cast<float>(Direct3D_GetBackBufferHeight());

    // 頂点シェーダーに変換行列を設定
    Shader_SetProjectionMatrix(XMMatrixOrthographicOffCenterLH(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f));
}

void Sprite_Draw(int texid, float display_x, float display_y, const XMFLOAT4& color)
{
    float IMAGE_WIDTH = static_cast<float>(Texture_Width(texid));
    float IMAGE_HEIGHT = static_cast<float>(Texture_Height(texid));

    Sprite_Draw(texid, display_x, display_y, 0.0f, 0.0f, IMAGE_WIDTH, IMAGE_HEIGHT, IMAGE_WIDTH, IMAGE_HEIGHT, color);
}

void Sprite_Draw(int texid, float display_x, float display_y, float display_w, float display_h, const XMFLOAT4& color)
{
    float IMAGE_WIDTH = static_cast<float>(Texture_Width(texid));
    float IMAGE_HEIGHT = static_cast<float>(Texture_Height(texid));

    Sprite_Draw(texid, display_x, display_y, 0.0f, 0.0f, IMAGE_WIDTH, IMAGE_HEIGHT, display_w, display_h, color);
}

void Sprite_Draw(int texid, float display_x, float display_y, float uvcut_x, float uvcut_y, float uvcut_w,
                 float uvcut_h, const XMFLOAT4& color)
{
    Sprite_Draw(texid, display_x, display_y, uvcut_x, uvcut_y, uvcut_w, uvcut_h, uvcut_w, uvcut_h, color);
}

void Sprite_Draw(int texid, float display_x, float display_y, float uvcut_x, float uvcut_y, float uvcut_w,
                 float uvcut_h, float display_w, float display_h,
                 const XMFLOAT4& color)
{
    // テクスチャ設定
    Texture_SetTexture(texid);

    // シェーダーを描画パイプラインに設定
    Shader_Begin();

    // 頂点バッファをロックする
    D3D11_MAPPED_SUBRESOURCE msr;
    g_pContext->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

    // 頂点バッファへの仮想ポインタを取得
    Vertex* v = static_cast<Vertex*>(msr.pData);

    // 画面の左上から右下に向かう線分を描画する -> 時計回り
    v[0].position = {display_x, display_y, 0.0f}; // LT
    v[1].position = {display_x + display_w, display_y, 0.0f}; // RT
    v[2].position = {display_x, display_y + display_h, 0.0f}; // LB
    v[3].position = {display_x + display_w, display_y + display_h, 0.0f}; // RB

    v[0].color = color;
    v[1].color = color;
    v[2].color = color;
    v[3].color = color;

    // UVマップ
    const int IMAGE_WIDTH = Texture_Width(texid);
    const int IMAGE_HEIGHT = Texture_Height(texid);

    float u0 = uvcut_x / static_cast<float>(IMAGE_WIDTH);
    float v0 = uvcut_y / static_cast<float>(IMAGE_HEIGHT);
    float u1 = (uvcut_x + uvcut_w) / static_cast<float>(IMAGE_WIDTH);
    float v1 = (uvcut_y + uvcut_h) / static_cast<float>(IMAGE_HEIGHT);

    v[0].uv = {u0, v0};
    v[1].uv = {u1, v0};
    v[2].uv = {u0, v1};
    v[3].uv = {u1, v1};

    // 頂点バッファのロックを解除
    g_pContext->Unmap(g_pVertexBuffer, 0);

    // 頂点バッファを描画パイプラインに設定
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

    // プリミティブトポロジ設定
    g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    // ポリゴン描画命令発行
    g_pContext->Draw(NUM_VERTEX, 0);
}
