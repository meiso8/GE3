#include "../Hlsli/Sprite.hlsli"


struct MaterialForFont
{
    float4 color;
    float32_t4x4 uvTransform;
    float temperature;
};

ConstantBuffer<MaterialForFont> gMaterial : register(b0);
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
    float4 tempColor = { 0.0, 0.0, 0.0, 0.0 };

    float3 redColor = float3(1.0, 0.0, 0.0);
    float threshold = 0.99;
    
    if (distance(textureColor.rgb, redColor) < threshold)
    {
        tempColor = float4(1.0, 1.0, 1.0, textureColor.r);
    }
    else if (length(textureColor.rgb) <= 0.1)
    { 
        discard;
    }
    
    output.color = gMaterial.color * tempColor;

    if (output.color.a <= 0.0)
    {
        discard;
    }
    
    return output;
}
