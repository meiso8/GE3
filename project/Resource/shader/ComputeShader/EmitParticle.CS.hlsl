
#include "../Hlsli/Particle.hlsli"
#include "../Hlsli/Random.hlsli"

ConstantBuffer<EmitterSphere> gEmitter : register(b0);
RWStructuredBuffer<Particle> gParticles : register(u0);
ConstantBuffer<PerFrame> gPerFrame : register(b1);

RWStructuredBuffer<int> gFreeListIndex : register(u1);
RWStructuredBuffer<uint32_t> gFreeList : register(u2);


[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    if (gEmitter.emit != 0)
    {
        RandomGenerator generator;
        generator.seed = (DTid + gPerFrame.time) * gPerFrame.time;
        
        for (uint countIndex = 0; countIndex < gEmitter.count; ++countIndex)
        {
            int freeListIndex;
            
            InterlockedAdd(gFreeListIndex[0],-1, freeListIndex);
            
            if (0 <= freeListIndex && freeListIndex < kMaxParticles)
            {
                uint32_t particleIndex = gFreeList[freeListIndex];
                
                gParticles[particleIndex].scale = generator.Generate3d();
                gParticles[particleIndex].translate = generator.Generate3d();
                gParticles[particleIndex].color.rgb = generator.Generate3d();
                gParticles[particleIndex].color.a = generator.Generate1d();
                 
                gParticles[particleIndex].currentTime = 0.0f;
                gParticles[particleIndex].lifeTime = generator.Generate1d();
                
                gParticles[particleIndex].velocity = generator.Generate3d() * 2.0f - 1.0f;
            }
            else
            {
                //Add1 Back Index
                InterlockedAdd(gFreeListIndex[0], 1);
                break;
            }
            
        

        }
    }
    
    
}