/**
 * @file shader_pixel_2d.hlsl
 * @brief 2D描画用ピクセルシェーダー
 * @author KOOLER FAN
 * @date 2025/06/10
 */

struct PS_INPUT
{
    float4 posH : SV_POSITION;
    float4 color : COLOR0;
    float2 uv : TEXCOORD0;
};

Texture2D tex : register(t0);
SamplerState samp : register(s0);

// HSV → RGB 轉換
float3 HSVtoRGB(float h, float s, float v)
{
    float3 rgb = float3(0, 0, 0);
    float c = v * s;
    float x = c * (1 - abs(fmod(h * 6, 2) - 1));
    float m = v - c;

    if (0 <= h && h < 1.0 / 6) rgb = float3(c, x, 0);
    else if (1.0 / 6 <= h && h < 2.0 / 6) rgb = float3(x, c, 0);
    else if (2.0 / 6 <= h && h < 3.0 / 6) rgb = float3(0, c, x);
    else if (3.0 / 6 <= h && h < 4.0 / 6) rgb = float3(0, x, c);
    else if (4.0 / 6 <= h && h < 5.0 / 6) rgb = float3(x, 0, c);
    else rgb = float3(c, 0, x);

    return rgb + m;
}

float4 main(PS_INPUT ps_in) : SV_TARGET
{
    // float hue = frac(ps_in.uv.x); // 0~1 之間循環
    float hue = frac(ps_in.uv.x + ps_in.uv.y);

    float3 rainbow = HSVtoRGB(hue, 1.0, 1.0);

    float4 texColor = tex.Sample(samp, ps_in.uv);
    float4 finalColor;
    finalColor.rgb = texColor.rgb * rainbow * ps_in.color.rgb;
    finalColor.a = texColor.a * ps_in.color.a;

    return finalColor;
}
