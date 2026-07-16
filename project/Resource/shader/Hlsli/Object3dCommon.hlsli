
#ifndef OBJECT_FUNCTIONS_HLSLI
#define OBJECT_FUNCTIONS_HLSLI

float4 GetTemperatureTexture(float objectAlpha, float temperature)
{
    //OutLineCancel
    float outlineMask = 1.0f - objectAlpha;
    outlineMask = clamp(outlineMask, 0.0f, 1.0f);
    //SetTemperature　Use G Channel for OutlineMask
    return float4(temperature, outlineMask, 0.0, 1.0);
};

#endif // OBJECT_FUNCTIONS_HLSLI