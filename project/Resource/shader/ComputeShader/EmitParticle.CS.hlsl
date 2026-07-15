
#include "../Hlsli/Particle.hlsli"

ConstantBuffer<EmitterSphere> gEmitter : register(b0);
RWStructuredBuffer<Particle> gParticles : register(u0);

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    if (gEmitter.emit != 0)
    {
        
        for (uint countIndex = 0; countIndex < gEmitter.count; ++countIndex)
        {
            gParticles[countIndex].scale = float32_t3(0.3f, 0.3f, 0.3f);
            gParticles[countIndex].translate = float32_t3(0.0f, 0.0f, 0.0f);
            gParticles[countIndex].color = float32_t4(1.0f, 0.0f, 0.0f, 1.0f);

        }
    }
    
    
}