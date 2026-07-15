#include "../Hlsli/Fullscreen.hlsli"
#include "../Hlsli/Random.hlsli"

//Texture2D<float4> gTexture : register(t2);

struct Material
{
    float time;
};

ConstantBuffer<Material> gMaterial : register(b0);
SamplerState gSampler : register(s0);


struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float random = rand2dTo1d(input.texcoord * gMaterial.time);
    output.color = float4(random, random, random, 1.0f);
    return output;
}