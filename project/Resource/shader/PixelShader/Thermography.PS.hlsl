#include "../Hlsli/Fullscreen.hlsli"

struct Material
{
    float alpha;
    int kernel;
    float sigma;
    float padding;
};

ConstantBuffer<Material> gMaterial : register(b0);
//empty thread is t2
Texture2D<float4> gTemperatureTexture : register(t2);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};


static const float32_t PI = 3.14159265f;

float gauss(float x, float y, float sigma)
{
    float exponent = -(x * x + y * y) * rcp(2.0f * sigma * sigma);
    float denominator = 2.0f * PI * sigma * sigma;
    return exp(exponent) * rcp(denominator);

}

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
    
    output.color = float4(0.0f, 0.0f, 0.0f,1.0f);

    //最初にぼかしをかける
    uint width, height;
    
    gTemperatureTexture.GetDimensions(width, height);
    float2 uvStepSize = float2(rcp(width), rcp(height));
     
    float weight = 0.0f;
   
    for (int x = -gMaterial.kernel; x < gMaterial.kernel; ++x)
    {
        for (int y = -gMaterial.kernel; y < gMaterial.kernel; ++y)
        {
            float2 offset = float2((float) x, (float) y);
            float kernel = gauss(offset.x, offset.y, gMaterial.sigma);
            weight += kernel;
            float2 texcoord = input.texcoord + offset * uvStepSize;
            float fetchColor = gTemperatureTexture.Sample(gSampler, texcoord).r;
            output.color.r += fetchColor * kernel;
        }
    }
     
    output.color.r *= rcp(weight);

    output.color.rgb = ThermalColorLookup(output.color.r);
    
    output.color.a *= gMaterial.alpha;

    return output;
}