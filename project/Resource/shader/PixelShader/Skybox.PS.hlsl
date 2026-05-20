#include "../Hlsli/Skybox.hlsli"

struct Material
{
    float4 color;
};

ConstantBuffer<Material> gMaterial : register(b0);
TextureCube<float4> gTexture : register(t2);
SamplerState gSampler : register(s0);


//struct PixelShaderOutput
//{
//    float4 color : SV_TARGET0;
//};


struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
    float4 temperature : SV_TARGET1; //AddTemperature
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float4 textureColor = gTexture.Sample(gSampler, input.texcoord);
    output.color = textureColor * gMaterial.color;
    //float temp = normalize(output.color.r + output.color.g + output.color.b);
    //temp *= 0.5;

    output.temperature = float4(output.color.r, 0, 0, 0);
    return output;
}