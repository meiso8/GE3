#include "../Hlsli/Sprite.hlsli"

struct Material
{
    float4 color;
    float32_t4x4 uvTransform;
    float temperature;
};


ConstantBuffer<Material> gMaterial : register(b0);
Texture2D<float4> gTexture : register(t2);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
    float4 temperature : SV_TARGET1; //AddTemperature
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    float outlineMask = 1.0f - gMaterial.color.a;
    outlineMask = clamp(outlineMask, 0.0f, 1.0f);
    //SetTemperature　Use G Channel for OutlineMask
    output.temperature = float4(gMaterial.temperature, outlineMask, 0.0, 1.0);
    
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    
    if (textureColor.a <= 0.2)
    {
        discard;
    }
    
    output.color = gMaterial.color * textureColor;
    
    if (output.color.a <= 0.0)
    {
        discard;
    }
    
    
    return output;
}
