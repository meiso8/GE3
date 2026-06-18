#include "../Hlsli/Object3d.hlsli"

struct MaterialForLine
{
    float4 color;
};

ConstantBuffer<MaterialForLine> gMaterial : register(b0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    output.color = gMaterial.color;
    return output;
}