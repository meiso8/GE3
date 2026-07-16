
#ifndef LIGHT_FUNCTIONS_HLSLI
#define LIGHT_FUNCTIONS_HLSLI
struct DirectionalLight
{
    float4 color;
    float3 direction;
    float intensity;
};

struct PointLight
{
    float4 color;
    float3 position;
    float intensity;
    float radius;
    float decay;
    float padding[2];
};

struct SpotLight
{
    float4 color;
    float3 position;
    float intensity;
    float3 direction;
    float distance;
    float decay;
    float cosAngle;
    float cosFalloffStart;
    float padding[1];
};

float GetCosin(float NdotL, int lightMode)
{
    return (lightMode == 1) ? saturate(NdotL) : pow(NdotL * 0.5f + 0.5f, 2.0f);
}

float3 CalculatePointLightDiffuse(float3 normal, float3 worldPos, PointLight light, int lightMode)
{
  
    float3 toWorldPos = worldPos - light.position;
    float3 dir = normalize(toWorldPos);
    float distance = length(toWorldPos);
    float factor = pow(saturate(-distance / light.radius + 1.0f), light.decay);
    float NdotL = dot(normal, -dir);
    float cos = GetCosin(NdotL, lightMode);
    return light.color.rgb * cos * light.intensity * factor;
    
}

float3 CalculateSpotLightDiffuse(float3 normal, float3 worldPos, SpotLight light, int lightMode)
{
  
    float3 toWorldPos = worldPos - light.position;
    float3 dir = normalize(toWorldPos);
    float cosAngle = dot(dir, normalize(light.direction));
    float falloffFactor = saturate((cosAngle - light.cosAngle) / (light.cosFalloffStart - light.cosAngle));
    float attenuationFactor = pow(saturate(light.distance / 1.0f), light.decay);
    return light.color.rgb * light.intensity * falloffFactor * attenuationFactor;
    
}

float3 CalculatePointLightSpecular(float3 normal, float3 worldPos, float3 toEye, PointLight light, float shininess)
{
    float3 toWorldPos = worldPos - light.position;
    float3 dir = normalize(toWorldPos);
    float3 halfVector = normalize(-dir + toEye);
    float NDotH = dot(normal, halfVector);
    float spec = pow(saturate(NDotH), shininess);
    return spec * light.color.rgb; //<-selectReflectColor
    
}

float3 CalculateSpotLightSpecular(float3 normal, float3 toEye, SpotLight light, float shininess)
{
    float3 halfVector = normalize(-light.direction + toEye);
    float NDotH = dot(normal, halfVector);
    float spec = pow(saturate(NDotH), shininess);
    return spec * light.color.rgb; //<-selectReflectColor
}


float3 CalculateDirectionalDiffuse(float3 normal, float3 dir, float3 color, float intensity, int lightMode)
{
    float NdotL = dot(normal, -dir);
    float cos = GetCosin(NdotL, lightMode);
    return color.rgb * cos * intensity;
}

float3 CalculateDirectionalSpecular(float3 normal, float3 dir, float3 toEye, float3 color, float shininess)
{
    float3 reflectLight = reflect(dir, normal);
    float3 halfVector = normalize(-dir + toEye);
    float NDotH = dot(normal, halfVector);
    float spec = pow(saturate(NDotH), shininess);
    return spec * color; //<-selectReflectColor
}

float4 GetCalculateAllLightColor(
int lightMode,
float shininess,
float4 materialColor,
float4 textureColor,
float3 objectNormal,
float3 objectWorldPos,
float3 toEye,
StructuredBuffer<PointLight> pointLights,
StructuredBuffer<SpotLight> spotLights,
DirectionalLight directionalLight

)
{
    float4 output;
    
     // ==========================//Common//====================================
        //normal
        float3 normalInput = normalize(objectNormal);
        //baseColor
        float3 baseColor = materialColor.rgb * textureColor.rgb;
     // ======================================================================
  
     // totalPointLightDiffuse
    float3 lightTotalDiffuse = float3(0, 0, 0);
    float3 lightTotalSpecular = float3(0, 0, 0);

     [loop]
    for (int i = 0; i < 20; ++i)
    {
        lightTotalDiffuse += CalculatePointLightDiffuse(normalInput, objectWorldPos, pointLights[i], lightMode);
        lightTotalDiffuse += CalculateSpotLightDiffuse(normalInput, objectWorldPos, spotLights[i], lightMode);
    }
               
    float3 DirectionalLightDiffuse = CalculateDirectionalDiffuse(normalInput, directionalLight.direction, directionalLight.color.rgb, directionalLight.intensity, lightMode);
        
    if (lightMode == 1)
    {
            
             [loop]
             for (int i = 0; i < 20; ++i)
             {
                 lightTotalSpecular += CalculatePointLightSpecular(normalInput, objectWorldPos, toEye, pointLights[i], shininess);
                 lightTotalSpecular += CalculateSpotLightSpecular(normalInput, toEye, spotLights[i], shininess);
             }
                            
                 //directionalLightReflect
             lightTotalSpecular +=
                 DirectionalLightDiffuse *
                 CalculateDirectionalSpecular(
                  normalInput,
                  directionalLight.direction,
                  toEye,
                  directionalLight.color.rgb,
                  shininess
                  );
                  
             output.rgb = baseColor * (DirectionalLightDiffuse + lightTotalDiffuse) + lightTotalSpecular;

                 
    }
    else
    {
     //NoneReflect
        output.rgb = baseColor * (DirectionalLightDiffuse + lightTotalDiffuse);
    }
        
    //commonLightMode
    output.a = materialColor.a * textureColor.a;
        
    return output;

}

#endif // LIGHT_FUNCTIONS_HLSLI