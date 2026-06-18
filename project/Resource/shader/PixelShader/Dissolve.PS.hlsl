
#include "../Hlsli/Fullscreen.hlsli"

struct Material
{
    float32_t maskVal;
    float32_t3 rgb;
};


ConstantBuffer<Material> gMaterial : register(b0);

Texture2D<float> gMaskTexture : register(t0);
Texture2D<float4> gTexture : register(t2);
SamplerState gSampler : register(s0);


struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    float mask = gMaskTexture.Sample(gSampler, input.texcoord);
    
    if (mask <= gMaterial.maskVal)
    {
        discard;
    }
 
    float32_t edge = 1.0f - smoothstep(0.5f, 0.53f, mask);
    
    
    PixelShaderOutput output;
    output.color = gTexture.Sample(gSampler, input.texcoord);
    output.color.rgb += edge * gMaterial.rgb;
    return output;
}