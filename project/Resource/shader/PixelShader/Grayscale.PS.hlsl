
#include "../Hlsli/Fullscreen.hlsli"


struct Material
{
    float4 color;
    bool useEffect;
    int type;
    float effectVol;
    float padding;
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
    PixelShaderOutput output;
    output.color = gTexture.Sample(gSampler, input.texcoord);
  
    if (gMaterial.useEffect)
    {
        float32_t value = dot(output.color.rgb, float32_t3(0.2125f, 0.715f, 0.0721f));
        float effectVol = clamp(gMaterial.effectVol, 0.0f, 1.0f);
        
        if (gMaterial.type == 0)
        {
        //sepia
            output.color.rgb = lerp(output.color.rgb, value * gMaterial.color.rgb, effectVol);
        }
        else if (gMaterial.type == 1)
        {
        //grayscale 
            output.color.rgb = lerp(output.color.rgb, float32_t3(value, value, value), effectVol);
        }
    }
   

    return output;
}