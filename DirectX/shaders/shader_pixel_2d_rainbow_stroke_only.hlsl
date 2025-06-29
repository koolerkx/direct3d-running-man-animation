struct PS_INPUT {
    float4 posH : SV_POSITION;
    float4 color : COLOR0;
    float2 uv : TEXCOORD0;
};

Texture2D tex; // テクスチャ
SamplerState samp; // テクスチャサンプラ

// Convert HSV to RGB
float3 hsv2rgb(float3 c)
{
    float4 K = float4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    float3 p = abs(frac(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * lerp(K.xxx, saturate(p - K.xxx), c.y);
}

float4 main(PS_INPUT ps_in) : SV_TARGET
{
    // Simple stroke detection using texture derivatives
    float2 texelSize = fwidth(ps_in.uv) * 2.0; // Adjust multiplier for stroke width

    // Sample surrounding pixels
    float alpha = 0.0;
    alpha += tex.Sample(samp, ps_in.uv + float2(texelSize.x, 0)).a;
    alpha += tex.Sample(samp, ps_in.uv + float2(-texelSize.x, 0)).a;
    alpha += tex.Sample(samp, ps_in.uv + float2(0, texelSize.y)).a;
    alpha += tex.Sample(samp, ps_in.uv + float2(0, -texelSize.y)).a;

    // Only stroke edges: alpha>0 且<4
    if (alpha > 0.0 && alpha < 4.0)
    {
        // Create rainbow based on UV coordinates
        float hue = frac((ps_in.uv.x + ps_in.uv.y) * 3.14159);
        float3 rainbowColor = hsv2rgb(float3(hue, 1.0, 1.0));

        return float4(rainbowColor, 1.0) * ps_in.color;
    }

    // Transparent background
    return float4(0.0, 0.0, 0.0, 0.0);
}
