#include "../Hlsli/Particle.hlsli"

struct Material
{
    float4 color;
    int32_t lightMode;
    float32_t shininess;
    float32_t environmentCoefficient;
    //温度を追加してみる
    float temperature;
    float32_t4x4 uvTransform;
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
    
    //SetTemperature　Use G Channel for OutlineMask
    output.temperature = float4(gMaterial.temperature, 1.0f, 0.0, 1.0);
    
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    output.color = gMaterial.color * textureColor*input.color; 

    if (textureColor.a == 0.0)
    {
        discard;
    }
    
    return output;
}
