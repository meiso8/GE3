#ifndef RANDOM_FUNCTIONS_HLSLI
#define RANDOM_FUNCTIONS_HLSLI

float rand2dTo1d(float2 value, float2 dotDir = float2(12.9898, 78.233))
{
    float2 samllValue = sin(value);
    float random = dot(samllValue, dotDir);
    random = frac(sin(random) * 143758.5453);
    return random;

}


float rand3dTo1d(float3 value, float3 dotDir = float3(12.9898, 78.233, 37.719))
{
    //make value smaller to avoid artefacts
    float3 smallValue = sin(value);
    //get scalar value from 3d vector
    float random = dot(smallValue, dotDir);
    //make value more random by making it bigger and then taking teh factional part
    random = frac(sin(random) * 143758.5453);
    return random;
}
 float3 rand3dTo3d(
float3 value)
{
    return float3(
        rand3dTo1d(value, float3(12.989, 78.233, 37.719)),
        rand3dTo1d(value, float3(39.346, 11.135, 83.155)),
        rand3dTo1d(value, float3(73.156, 52.235, 09.151))
    );
}


class RandomGenerator
{
    float3 seed;
    float3 Generate3d()
    {
        seed = rand3dTo3d(seed);
        return seed;
    }
    
    float Generate1d()
    {
        float result = rand3dTo1d(seed);
        seed.x = result;
        return result;
    }
};

#endif // RANDOM_FUNCTIONS_HLSLI