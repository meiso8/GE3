
#include "../Hlsli/SkinningObject.hlsli"

#include "../Hlsli/WaveData.hlsli"
#include "../Hlsli/BallonData.hlsli"
ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b1);
ConstantBuffer<Balloon> gBalloon : register(b5);
StructuredBuffer<Wave> gWave : register(t1);
StructuredBuffer<Well> gMatrixPalette : register(t6);


Skinned Skinning(VertexShaderInput input)
{
    Skinned skinned;
    //位置の変換
    skinned.position = mul(input.position, gMatrixPalette[input.index.x].skeletonSpaceMatrix) * input.weight.x;
    skinned.position += mul(input.position, gMatrixPalette[input.index.y].skeletonSpaceMatrix) * input.weight.y;
    skinned.position += mul(input.position, gMatrixPalette[input.index.z].skeletonSpaceMatrix) * input.weight.z;
    skinned.position += mul(input.position, gMatrixPalette[input.index.w].skeletonSpaceMatrix) * input.weight.w;
    skinned.position.w = 1.0f;
    
    //法線の変換
    skinned.normal = mul(input.normal, (float32_t3x3) gMatrixPalette[input.index.x].skeletonSpaceMatrix) * input.weight.x;
    skinned.normal += mul(input.normal, (float32_t3x3) gMatrixPalette[input.index.y].skeletonSpaceMatrix) * input.weight.y;
    skinned.normal += mul(input.normal, (float32_t3x3) gMatrixPalette[input.index.z].skeletonSpaceMatrix) * input.weight.z;
    skinned.normal += mul(input.normal, (float32_t3x3) gMatrixPalette[input.index.w].skeletonSpaceMatrix) * input.weight.w;
    skinned.normal = normalize(skinned.normal);
    
    
    return skinned;

}


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

    Skinned skinned = Skinning(input);
    input.position = skinned.position;
    input.normal = skinned.normal;
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

    output.worldPosition = mul(input.position, gTransformationMatrix.World).xyz;
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float32_t3x3) gTransformationMatrix.WorldInverseTranspose));

    return output;
}