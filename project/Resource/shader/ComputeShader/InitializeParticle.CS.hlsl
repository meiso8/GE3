

#include "../Hlsli/Particle.hlsli"

RWStructuredBuffer<Particle> gParticles : register(u0);
RWStructuredBuffer<int> gFreeListIndex : register(u1);
RWStructuredBuffer<uint32_t> gFreeList : register(u2);

[numthreads(1024, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    
    uint particleIndex = DTid.x;
    
    //initialize ListAndParticle
    if (particleIndex < kMaxParticles)
    {
        gParticles[particleIndex] = (Particle) 0;
        gFreeList[particleIndex] = particleIndex;
    }
    
    if (particleIndex == 0)
    {
        gFreeListIndex[0] = 0;

    }

    
}