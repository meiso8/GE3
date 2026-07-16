

#ifndef BALOON_FUNCTIONS_HLSLI
#define BALOON_FUNCTIONS_HLSLI
struct Balloon
{
    float expansion;
    float sphere;
    float cube;
    bool isSphere;
};
float3 BalloonUpdate(float3 normal, Balloon balloon)
{
    return normal * balloon.expansion;
}

float4 SphereUpdate(float4 position, Balloon balloon)
{
    float4 output;
    
    output.xyz = lerp(position.xyz, normalize(position.xyz), balloon.sphere);
    output.w = position.w;
    
    return output;
}

float4 CubeUpdate(float4 position, Balloon balloon)
{
    float4 output;
    
    output.xyz = lerp(position.xyz, clamp(normalize(position.xyz), -0.5f, 0.5f), balloon.cube);
    output.w = position.w;
    
    return output;
}

float4 UpdateExpansionData(float4 position, float3 normal,Balloon balloon,float4x4 WVP)
{
    position.xyz += BalloonUpdate(normal, balloon);

    if (balloon.isSphere)
    {
        return mul(SphereUpdate(position, balloon), WVP);
    }
    else
    {
        return mul(CubeUpdate(position, balloon), WVP);
    }

}

#endif // BALOON_FUNCTIONS_HLSLI