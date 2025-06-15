/**
 * @file shader_vertex_2d.hlsl
 * @brief 2D描画用頂点シェーダー
 * @author KOOLER FAN
 * @date 2025/06/10
 */

// 定数バッファ
float4x4 mtx;

struct VS_INPUT
{
    float4 posL : POSITION0; // ローカル座標 
    float4 color : COLOR0; // ? 
};

struct VS_OUTPUT
{
    float4 posH : SV_POSITION; // 変換済み座標 
    float4 color : COLOR0; // ? 
};

// 頂点シェーダ
VS_OUTPUT main(VS_INPUT vs_in)
{
    VS_OUTPUT vs_out;
    vs_out.posH = mul(vs_in.posL, mtx);
    vs_out.color = vs_in.color;
    return vs_out;
}
