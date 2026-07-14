
#include "../Hlsli/Particle.hlsli"

StructuredBuffer<ParticleForGPU> gParticle : register(t3);

VertexShaderOutput main(VertexShaderInput input,uint32_t instanceId : SV_InstanceID)
{
    VertexShaderOutput output;

    output.position = mul(input.position, gParticle[instanceId].WVP);
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float3x3) gParticle[instanceId].World));
    output.color = gParticle[instanceId].color;

    return output;
}