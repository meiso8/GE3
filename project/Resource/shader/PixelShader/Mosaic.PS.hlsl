
#include "../Hlsli/Fullscreen.hlsli"

struct Material
{
    bool useMosaic;
    int size;
    float vol;
    float padding;
};

ConstantBuffer<Material> gMaterial : register(b0);
Texture2D<float4> gTexture : register(t2);
SamplerState gSampler : register(s0);

float2 Posterize_float2(float2 In, float2 Steps)
{
    return floor(In / (1 / Steps)) * (1 / Steps);	
}

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    if (gMaterial.useMosaic)
    {
        uint32_t width, height;
        gTexture.GetDimensions(width, height);
        float sizeY = height * rcp(width) * gMaterial.size;

        float2 mosaic = Posterize_float2(input.texcoord, float2(gMaterial.size, sizeY));
        mosaic += gMaterial.size;
        output.color = lerp(gTexture.Sample(gSampler, input.texcoord), gTexture.Sample(gSampler, mosaic), clamp(gMaterial.vol, 0, 1));
    }
    else
    {
        output.color = gTexture.Sample(gSampler, input.texcoord);
    }

    return output;
}