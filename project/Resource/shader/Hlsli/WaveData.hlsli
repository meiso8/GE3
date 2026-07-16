
#ifndef WAVE_FUNCTIONS_HLSLI
#define WAVE_FUNCTIONS_HLSLI

struct Wave
{
    float3 direction;
    float time;
    float amplitude;
    float frequency;
};

float WaveCreate(float4 position, Wave wave)
{
    float Dot = dot(position.xyz, normalize(wave.direction) * wave.frequency);
    return cos(wave.time + Dot) * wave.amplitude; 
}

float WaveUpdate(float4 position,Wave wave1,Wave wave2)
{
    return WaveCreate(position, wave1) + WaveCreate(position,wave2);
}
#endif // WAVE_FUNCTIONS_HLSLI