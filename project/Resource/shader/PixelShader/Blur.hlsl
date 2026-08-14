#include "../Hlsli/object3d.hlsli"

struct Material
{
    float4 color;
};

struct DoFParam
{
    float sigma;
    uint kernel;
    float focusDepth;
};

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DoFParam> gDoFParam : register(b1);

Texture2D<float4> gTexture : register(t0); 
Texture2D<float4> gDepthTexture : register(t1)

SamplerState gSampler : register(s0); //Sampler


float GaussianWeight(float x, float sigma)
{
    float k = 1.0 / (sigma * sqrt(2.0 * 3.14159265359));
    return k * exp(-(x * x) / (2.0 * sigma * sigma));
}

float4 HorizontalBlur(Texture2D sceneTex, SamplerState sample, float2 uv, float2 offset, float sigma, uint kernel)
{
    float4 color = { 0.0f, 0.0f, 0.0f, 0.0f };
    

    for (int i = -int(kernel - 1 / 2); i <= int(kernel - 1 / 2); i++)
    {
        color +=
        sceneTex.Sample(sample, uv + float2(i * offset.x, 0)) * GaussianWeight(i, sigma);
    }
    
    return color;
}

float4 VerticalBlur(Texture2D sceneTex, SamplerState sample, float2 uv, float2 offset, float sigma, uint kernel)
{
    float4 color = { 0.0f, 0.0f, 0.0f, 0.0f };
     
    for (int i = -int(kernel - 1 / 2); i <= int(kernel - 1 / 2); i++) 
    {
        color += sceneTex.Sample(
        sample, uv
        + float2(0, i * offset.y)) * GaussianWeight(i, sigma);
    }
    return color;
}

float4 GaussianBlur(Texture2D sceneTex, SamplerState sample, float2 uv, float sigma, uint kernel)
{
    float2 offset = float2(1.0 / 1280.0, 1.0 / 720.0);

    float4 color = { 0.0f, 0.0f, 0.0f, 0.0f };
    
    color = HorizontalBlur(sceneTex, sample, uv, offset, sigma, kernel);
    color += VerticalBlur(sceneTex, sample, uv, offset, sigma, kernel);

    return color;
}



struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};


PixelShaderOutput main(VertexShaderOutput input)
{
 
    PixelShaderOutput output;
    
    float depth = gDepthTexture.Sample(gSampler, input.texcoord).r;

    float blurFactor = abs(depth - gDoFParam.focusDepth);

    float4 originalColor = gTexture.Sample(gSampler, input.texcoord);

    float4 blurColor = GaussianBlur(gTexture, gSampler, input.texcoord, gDoFParam.sigma, gDoFParam.kernel);
    

    output.color = lerp(originalColor, blurColor, blurFactor);
    
    //output.color = blurColor;
    
    //output.color = gMaterial.color * lerp(originalColor, blurColor, blurFactor);
    
    return output;
}

