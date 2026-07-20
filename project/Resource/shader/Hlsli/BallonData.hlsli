

#ifndef BALOON_FUNCTIONS_HLSLI
#define BALOON_FUNCTIONS_HLSLI
struct Balloon
{
    float expansion;
    float sphere;
    float cube;
    bool isSphere;
};
float3 BalloonUpdate(float3 normal, float expansion)
{
    return normal * expansion;
}

float4 SphereUpdate(float4 position, float size,float sphere)
{
    float4 output;
    
    output.xyz = lerp(position.xyz, normalize(position.xyz)*size, sphere);
    output.w = position.w;
    
    return output;
}

float4 CubeUpdate(float4 position, float cube)
{
    float4 output;
    
    output.xyz = lerp(position.xyz, clamp(normalize(position.xyz), -0.5f, 0.5f), cube);
    output.w = position.w;
    
    return output;
}

float4 UpdateExpansionData(float4 position, float3 normal,Balloon balloon)
{
    position.xyz += BalloonUpdate(normal, balloon.expansion);

    if (balloon.isSphere)
    {
        return SphereUpdate(position,1.0f, balloon.sphere);
    }
    else
    {
        return CubeUpdate(position, balloon.cube);
    }

}

#endif // BALOON_FUNCTIONS_HLSLI