
#include "../Hlsli/Object3d.hlsli"
#include "../Hlsli/WaveData.hlsli"
#include "../Hlsli/BallonData.hlsli"

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b1);
ConstantBuffer<Balloon> gBalloon : register(b5);
StructuredBuffer<Wave> gWave : register(t1);


VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;

    input.position.y += WaveUpdate(input.position,gWave[0],gWave[1]);
     
    output.position = UpdateExpansionData(input.position, input.normal, gBalloon, gTransformationMatrix.WVP);
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float3x3) gTransformationMatrix.WorldInverseTranspose));
    output.worldPosition = mul(input.position, gTransformationMatrix.World).xyz;
    return output;
}