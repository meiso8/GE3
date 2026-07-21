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

float4 MeltStart(float4 position, float3 normal, float thickness, float size, float melt)
{

    //position = SphereUpdate(position, 1.0f, melt);


    bool isMelting = melt >= 0.5f;
    float meltTimer = lerp(0, clamp((melt - 0.5f) * rcp(0.5f), 0.0f, 1.0f), isMelting);
    position.y += normal.y * thickness * lerp(1.0f, size, meltTimer);
    position.xz += normal.xz * melt * lerp(1.0f, size, meltTimer);

    position = SphereUpdate(position, size, meltTimer);
    
    position.y = lerp(position.y, thickness, melt);
    
    return position;
}

float4 MeltUpdate(float3 normal, float4 position, Melt melt)
{
    
    melt.time = min(melt.time, melt.meltTime * 3.0f - melt.thickness);
    
    if (position.y >= 0.5f)
    {
        position.y += WaveCreate(position, float3(0.0f, 1.0f, 0.0f), melt.time, melt.thickness * melt.time * 0.25f, 20);
        
    }
    
    float meltTimer = clamp((melt.time - melt.meltTime) * rcp(melt.meltTime * 2.0f), 0.0f, 1.0f);
    
    bool isMeltStart = melt.time >= melt.meltTime;
    
    return lerp(position, MeltStart(position, normal, melt.thickness, melt.size, meltTimer), lerp(0, meltTimer, isMeltStart));

}

#endif // MELT_FUNCTIONS_HLSLI