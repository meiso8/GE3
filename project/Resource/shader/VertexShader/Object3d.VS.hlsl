
#include "../Hlsli/Object3d.hlsli"
#include "../Hlsli/WaveData.hlsli"
#include "../Hlsli/BallonData.hlsli"
struct TransformationMatrix
{
    float32_t4x4 WVP;
    float32_t4x4 World;
    float32_t4x4 WorldInverseTranspose;
};


ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b1);
ConstantBuffer<Balloon> gBalloon : register(b5);
StructuredBuffer<Wave> gWave : register(t1);


struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

float WaveUpdate(VertexShaderInput input)
{
     
    float Dot1 = dot(input.position.xyz, normalize(gWave[0].direction) * gWave[0].frequency);
    float Wave1 = cos(gWave[0].time + Dot1) * gWave[0].amplitude;
    
    float Dot2 = dot(input.position.xyz, normalize(gWave[1].direction) * gWave[1].frequency);
    float Wave2 = cos(gWave[1].time + Dot2) * gWave[1].amplitude;
   
    return Wave1 + Wave2;
}

float3 BalloonUpdate(VertexShaderInput input)
{
    float3 output;
    
    output = input.normal * gBalloon.expansion;
    return output;
}

float4 SphereUpdate(VertexShaderInput input)
{
    float4 output;
    
    output.xyz = lerp(input.position.xyz, normalize(input.position.xyz), gBalloon.sphere);
    output.w = input.position.w;
    
    return output;
}

float4 CubeUpdate(VertexShaderInput input)
{
    float4 output;
    
    output.xyz = lerp(input.position.xyz, clamp(normalize(input.position.xyz), -0.5f, 0.5f), gBalloon.cube);
    output.w = input.position.w;
    
    return output;
}


VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;

    input.position.y += WaveUpdate(input);
     
    input.position.xyz += BalloonUpdate(input);

    if (gBalloon.isSphere)
    {
        output.position = mul(SphereUpdate(input), gTransformationMatrix.WVP);
    }
    else
    {
        output.position = mul(CubeUpdate(input), gTransformationMatrix.WVP);
    }

    
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float3x3) gTransformationMatrix.WorldInverseTranspose));
    output.worldPosition = mul(input.position, gTransformationMatrix.World).xyz;
    return output;
}