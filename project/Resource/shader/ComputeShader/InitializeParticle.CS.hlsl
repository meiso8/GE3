

#include "../Hlsli/Particle.hlsli"

static const uint kMaxParticles = 1024;

RWStructuredBuffer<Particle> gParticles : register(u0);

[numthreads(1024, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    
    uint particleIndex = DTid.x;
    
      //InitializeParticle
    if (particleIndex < kMaxParticles)
    {
        //All 0 Clear
        gParticles[particleIndex] = (Particle) 0;
        
        gParticles[particleIndex].scale = float32_t3(0.5f, 0.5f, 0.5f);
        gParticles[particleIndex].color = float32_t4(1.0f, 1.0f, 1.0f, 1.0f);
    }
    
    
}