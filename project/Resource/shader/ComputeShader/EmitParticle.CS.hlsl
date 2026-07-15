
#include "../Hlsli/Particle.hlsli"
#include "../Hlsli/Random.hlsli"

ConstantBuffer<EmitterSphere> gEmitter : register(b0);
RWStructuredBuffer<Particle> gParticles : register(u0);
ConstantBuffer<PerFrame> gPerFrame : register(b1);

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    if (gEmitter.emit != 0)
    {
        RandomGenerator generator;
        generator.seed = (DTid + gPerFrame.time) * gPerFrame.time;
        
        for (uint countIndex = 0; countIndex < gEmitter.count; ++countIndex)
        {
            gParticles[countIndex].scale = generator.Generate3d();
            gParticles[countIndex].translate = generator.Generate3d();
            gParticles[countIndex].color.rgb = generator.Generate3d();
            gParticles[countIndex].color.a = generator.Generate1d();
            
            gParticles[countIndex].currentTime = 0.0f;
            gParticles[countIndex].lifeTime = generator.Generate1d();
            gParticles[countIndex].velocity = generator.Generate3d();

        }
    }
    
    
}