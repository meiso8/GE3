
#include "../Hlsli/Particle.hlsli"

//t11
StructuredBuffer<Particle> gParticles : register(t11);
ConstantBuffer<ParView> gParView : register(b0);

VertexShaderOutput main(VertexShaderInput input, uint instanceId : SV_InstanceID)
{
    VertexShaderOutput output;

    Particle particle = gParticles[instanceId];
    float4x4 worldMatrix = gParView.billboardMatrix;
    
    worldMatrix[0] *= particle.scale.x;
    worldMatrix[1] *= particle.scale.y;
    worldMatrix[2] *= particle.scale.z;
    worldMatrix[3].xyz = particle.translate;
    
    
    output.position = mul(input.position, mul(worldMatrix,gParView.viewProjection));
    output.texcoord = input.texcoord;
    //Skup Normal
    output.color = particle.color;
    output.normal = float3(0.0f, 0.0f, -1.0f);
    return output;
}