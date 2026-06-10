#include "../Hlsli/Object3d.hlsli"

struct TransformationMatrix
{
    float32_t4x4 WVP;
    float32_t4x4 World;
    float32_t4x4 WorldInverseTranspose;
};

struct Point
{
    float3 startPos;
    float padding1;
    float3 endPos;
    float padding2;
};

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);
ConstantBuffer<Point> gPoint : register(b1);

struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};


VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;  
    
    float4 positionOffset;
    
    if (input.position.z < 0.0f)
    {
        positionOffset = float4(gPoint.startPos, 1.0f);
    }
    else
    {
        positionOffset = float4(gPoint.endPos, 1.0f);
    }
    output.position = mul(input.position + positionOffset, gTransformationMatrix.WVP);
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float3x3) gTransformationMatrix.WorldInverseTranspose));
    output.worldPosition = mul(input.position, gTransformationMatrix.World).xyz;
    return output;
}