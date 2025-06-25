#ifndef POLYGON_BASE_H
#define POLYGON_BASE_H

#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

#include <direct3d.h>
#include <debug_ostream.h>
#include "shader.h"

struct Vertex
{
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT4 color;
    DirectX::XMFLOAT2 texcoord;
};

class PolygonBase
{
public:
    PolygonBase(ID3D11Device* device, ID3D11DeviceContext* context, int vertexCount)
        : device_(device), context_(context), vertex_count_(vertexCount)
    {
        initializeBuffer();
    }

    virtual ~PolygonBase()
    {
    }

    PolygonBase(const PolygonBase&) = delete;
    PolygonBase& operator=(const PolygonBase&) = delete;

protected:
    ID3D11Device* device_ = nullptr;
    ID3D11DeviceContext* context_ = nullptr;
    ComPtr<ID3D11Buffer> vertex_buffer_;
    int vertex_count_ = 0;

    void initializeBuffer()
    {
        // デバイスとデバイスコンテキストのチェック
        if (!device_ || !context_)
        {
            hal::dout << "PolygonBase::InitializeBuffer() : 与えられたデバイスかコンテキストが不正です" << std::endl;
            return;
        }

        if (vertex_count_ <= 0) return;

        D3D11_BUFFER_DESC bd = {};
        bd.Usage = D3D11_USAGE_DYNAMIC;
        bd.ByteWidth = sizeof(Vertex) * vertex_count_;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        HRESULT hr = device_->CreateBuffer(&bd, nullptr, vertex_buffer_.GetAddressOf());
        if (FAILED(hr))
        {
            hal::dout << "PolygonBase::InitializeBuffer() : バッファ生成が失敗した" << std::endl;
            return;
        }
    }

    template <typename TCallback>
    void drawBase(TCallback&& drawCallback, int drawVertexCount)
    {
        // シェーダーを描画パイプラインに設定
        Shader_Begin();

        // 頂点バッファをロックする
        D3D11_MAPPED_SUBRESOURCE msr;
        context_->Map(vertex_buffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

        // 頂点バッファへの仮想ポインタを取得
        Vertex* v = static_cast<Vertex*>(msr.pData);

        // 頂点情報を書き込み
        drawCallback(v, drawVertexCount);

        // 頂点バッファのロックを解除
        context_->Unmap(vertex_buffer_.Get(), 0);

        // 頂点バッファを描画パイプラインに設定
        UINT stride = sizeof(Vertex);
        UINT offset = 0;
        context_->IASetVertexBuffers(0, 1, vertex_buffer_.GetAddressOf(), &stride, &offset);

        // 頂点シェーダーに変換行列を設定
        context_->IASetPrimitiveTopology(getTopology());

        // プリミティブトポロジ設定
        float SCREEN_WIDTH = static_cast<float>(Direct3D_GetBackBufferWidth());
        float SCREEN_HEIGHT = static_cast<float>(Direct3D_GetBackBufferHeight());
        Shader_SetProjectionMatrix(XMMatrixOrthographicOffCenterLH(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f));
        Shader_SetWorldMatrix(XMMatrixIdentity());
        
        // ポリゴン描画命令発行
        context_->Draw(vertex_count_, 0);
    }

    virtual D3D11_PRIMITIVE_TOPOLOGY getTopology() const = 0;
};

struct CircleProp
{
    float centerX = 0;
    float centerY = 0;
    float max_radius = 500.0f;
};

class CircleRenderer : public PolygonBase
{
public:
    CircleRenderer(ID3D11Device* device, ID3D11DeviceContext* context, CircleProp prop)
        : PolygonBase(device, context, static_cast<int>(prop.max_radius * 2.0f * XM_PI)),
          cx_(prop.centerX), cy_(prop.centerY), max_radius_(prop.max_radius)
    {
    }

    void draw(float radius)
    {
        if (radius > max_radius_)
        {
            hal::dout << "CircleRenderer::draw() 描く円の半径が最大半径を超えた" << std::endl;
            return;
        }

        int current_vertex_count = static_cast<int>(radius * 2.0f * XM_PI);

        drawBase([&](Vertex* v, int vertex_count)
        {
            const float rad = XM_2PI / vertex_count;
            for (int i = 0; i < vertex_count; i++)
            {
                v[i].position.x = cosf(rad * i) * radius + cx_;
                v[i].position.y = sinf(rad * i) * radius + cy_;
                v[i].position.z = 0.0f;
                v[i].color = {1.0f, 1.0f, 1.0f, 1.0f};
                v[i].texcoord = {0.0f, 1.0f};
            }
        }, current_vertex_count);
    }

protected:
    D3D11_PRIMITIVE_TOPOLOGY getTopology() const override
    {
        return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
    }

private:
    float cx_;
    float cy_;
    float max_radius_;
};


#endif // POLYGON_BASE_H
