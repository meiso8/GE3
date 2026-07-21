#ifndef MELT_FUNCTIONS_HLSLI
#define MELT_FUNCTIONS_HLSLI
#include"BallonData.hlsli"
#include"WaveData.hlsli"

struct Melt
{
    float time;
    float size;
    float meltTime;
    float thickness;
    
};

float4 MeltStart(float4 position, float3 normal, float thickness, float size,float melt)
{
    position = SphereUpdate(position, size,melt);
    position.y = lerp(position.y, thickness, melt);
    position.y += normal.y * thickness;
    position.xz += normal.xz * melt * lerp(1.0f, size, melt);
    return position;
}

float4 MeltUpdate(float3 normal, float4 position, Melt melt)
{
    melt.time = min(melt.time, melt.meltTime * 3.0f - melt.thickness);
    
    if (position.y >= melt.thickness)
    {
        position.y += WaveCreate(position, float3(0.0f, 1.0f, 0.0f), melt.time, melt.thickness * melt.time * 0.25f, 20);
        
    }
    


    float meltTimer = clamp((melt.time - melt.meltTime) * rcp(melt.meltTime * 2.0f), 0.0f, 1.0f);
    
    bool isMeltStart = melt.time >= melt.meltTime;
    
    return lerp(position, MeltStart(position, normal, melt.thickness, melt.size,meltTimer), lerp(0, meltTimer, isMeltStart));

}

#endif // MELT_FUNCTIONS_HLSLI