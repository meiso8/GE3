
#include "../Hlsli/Particle.hlsli"
#include "../Hlsli/Random.hlsli"

ConstantBuffer<EmitterSphere> gEmitter : register(b0);
RWStructuredBuffer<Particle> gParticles : register(u0);
ConstantBuffer<PerFrame> gPerFrame : register(b1);
RWStructuredBuffer<int> gFreeCounter : register(u1);

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    if (gEmitter.emit != 0)
    {
        RandomGenerator generator;
        generator.seed = (DTid + gPerFrame.time) * gPerFrame.time;
        
        for (uint countIndex = 0; countIndex < gEmitter.count; ++countIndex)
        {
            int particleIndex;
            
            InterlockedAdd(gFreeCounter[0], 1, particleIndex);
            
            if (particleIndex < kMaxParticles)
            {
                gParticles[particleIndex].scale = generator.Generate3d();
                gParticles[particleIndex].translate = generator.Generate3d();
                gParticles[particleIndex].color.rgb = generator.Generate3d();
                gParticles[particleIndex].color.a = generator.Generate1d();
                
                gParticles[particleIndex].currentTime = 0.0f;
                gParticles[particleIndex].lifeTime = generator.Generate1d();
                gParticles[particleIndex].velocity = generator.Generate3d();
            }

        }
    }
    
    
}