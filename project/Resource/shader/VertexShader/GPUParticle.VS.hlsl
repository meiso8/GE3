
#include "../Hlsli/Particle.hlsli"

float4 main( float4 pos : POSITION ) : SV_POSITION
{
	return pos;
}

//t11
StructuredBuffer<Particle> gParticles : register(t11);
ConstantBuffer<ParView> gParView : register(b0);

VertexShaderOutput main(VertexShaderInput input, uint32_t instanceId : SV_InstanceID)
{
    VertexShaderOutput output;

    Particle particle = gParticles[instanceId];
    float32_t4x4 worldMatrix = gParView.billboardMatrix;
    
    worldMatrix[0] *= particle.scale.x;
    worldMatrix[1] *= particle.scale.y;
    worldMatrix[2] *= particle.scale.z;
    worldMatrix[3].xyz *= particle.translate;
    
    
    output.position = mul(input.position, mul(worldMatrix,gParView.viewProjection));
    output.texcoord = input.texcoord;
    //Skup Normal
    output.color = particle.color;

    return output;
}