
#ifndef WAVE_FUNCTIONS_HLSLI
#define WAVE_FUNCTIONS_HLSLI

struct Wave
{
    float3 direction;
    float time;
    float amplitude;
    float frequency;
};

float WaveCreate(float4 position, float3 direction, float time, float amplitude, float frequency)
{
    float Dot = dot(position.xyz, normalize(direction) * frequency);
    return cos(time + Dot) * amplitude; 
}

float WaveCreateFromWaveData(float4 position, Wave wave)
{
   return WaveCreate(position,wave.direction,wave.time,wave.amplitude,wave.frequency);
}

float WaveUpdate(float4 position,Wave wave1,Wave wave2)
{
    return WaveCreateFromWaveData(position, wave1) + WaveCreateFromWaveData(position, wave2);
}
#endif // WAVE_FUNCTIONS_HLSLI
