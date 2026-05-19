#include "../Hlsli/Fullscreen.hlsli"

struct Material
{
    float4 color;
};

ConstantBuffer<Material> gMaterial : register(b0);
//empty thread is t2
Texture2D<float4> gTemperatureTexture : register(t2);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

float3 ThermalColorLookup(float temp)
{
    // 簡易的なジェットカラースケール（青 -> 緑 -> 赤）
    float3 color;
    color.r = saturate(1.5 - abs(4.0 * temp - 3.0));
    color.g = saturate(1.5 - abs(4.0 * temp - 2.0));
    color.b = saturate(1.5 - abs(4.0 * temp - 1.0));
    return color;
}

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    float4 rawTemp = gTemperatureTexture.Sample(gSampler, input.texcoord);
   
    if (rawTemp.r <= 0.0)
    {
        output.color = float4(0.0, 0.0, 1.0, 1.0f);
    }
    else
    {
        output.color.rgb = ThermalColorLookup(rawTemp.r);
        output.color.a = 1.0f;

    }
    
    output.color *= gMaterial.color;

    return output;
}