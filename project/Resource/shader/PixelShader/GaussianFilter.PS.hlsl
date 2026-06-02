#include "../Hlsli/Fullscreen.hlsli"

static const float32_t PI = 3.14159265f;

float gauss(float x, float y, float sigma)
{
    float exponent = -(x * x + y * y) * rcp(2.0f * sigma * sigma);
    float denominator = 2.0f * PI * sigma * sigma;
    return exp(exponent) * rcp(denominator);

}


struct Material
{
    int32_t kernel;
    float sigma;
};

ConstantBuffer<Material> gMaterial : register(b0);
Texture2D<float4> gTexture : register(t2);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    
    uint width, height;
    
    gTexture.GetDimensions(width, height);
    float2 uvStepSize = float2(rcp(width), rcp(height));
     
    float weight = 0.0f;
    PixelShaderOutput output;
    
    output.color.rgb = float3(0.0f, 0.0f, 0.0f);
    output.color.a = 1.0f;
    
    for (int x = -gMaterial.kernel; x <= gMaterial.kernel; ++x)
    {
        for (int y = -gMaterial.kernel; y <= gMaterial.kernel; ++y)
        {
            float2 offset = float2((float) x, (float) y);
            float kernel = gauss(offset.x, offset.y, gMaterial.sigma);
            weight += kernel;
            float2 texcoord = input.texcoord + offset * uvStepSize;
            float3 fetchColor = gTexture.Sample(gSampler, texcoord).rgb;
            output.color.rgb += fetchColor * kernel;
        }
    }
      
    output.color.rgb *= rcp(weight);
    
    return output;
}