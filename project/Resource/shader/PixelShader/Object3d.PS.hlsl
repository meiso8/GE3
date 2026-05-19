#include "../Hlsli/Object3d.hlsli"
#include "../Hlsli/Camera.hlsli"
#include "../Hlsli/Light.hlsli"

struct Material
{
    float4 color;
    int32_t lightMode;
    float32_t shininess;
    float32_t environmentCoefficient;
    float temperature;
    float32_t4x4 uvTransform;
};

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);
ConstantBuffer<Camera> gCamera : register(b2);

Texture2D<float4> gTexture : register(t2);
SamplerState gSampler : register(s0);

StructuredBuffer<PointLight> gPointLights : register(t4);
StructuredBuffer<SpotLight> gSpotLights : register(t5);
TextureCube<float4> gEnvironmentTexture : register(t7);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
    float4 temperature : SV_TARGET1;//AddTemperature
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

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    //SetTemperature
    output.temperature = float4(gMaterial.temperature, 0.0, 0.0, 1.0);
   
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
   
    if (textureColor.a <= 0.1)
    {
        discard;
    }
    
    if (gMaterial.lightMode == 0)
    {
        output.color = gMaterial.color * textureColor;
     
    }
    else
    {
 
        // ==========================//Common//====================================
        //normal
        float3 normalInput = normalize(input.normal);
        //baseColor
        float3 baseColor = gMaterial.color.rgb * textureColor.rgb;
         //toCameraVector
        float3 toEye = normalize(gCamera.worldPosition - input.worldPosition);

        // ======================================================================
  
        // totalPointLightDiffuse
        float3 lightTotalDiffuse = float3(0, 0, 0);
        float3 lightTotalSpecular = float3(0, 0, 0);

     [loop]
        for (int i = 0; i < 20; ++i)
        {
            lightTotalDiffuse += CalculatePointLightDiffuse(normalInput, input.worldPosition, gPointLights[i], gMaterial.lightMode);
        }
        
        
        float3 DirectionalLightDiffuse = CalculateDirectionalDiffuse(normalInput, gDirectionalLight.direction, gDirectionalLight.color.rgb, gDirectionalLight.intensity, gMaterial.lightMode);

           [loop]
        for (int i = 0; i < 20; ++i)
        {
            lightTotalDiffuse += CalculateSpotLightDiffuse(normalInput, input.worldPosition, gSpotLights[i], gMaterial.lightMode);
        }
        
        if (gMaterial.lightMode == 1)
        {
            
          [loop]
            for (int i = 0; i < 20; ++i)
            {
                lightTotalSpecular += CalculatePointLightSpecular(normalInput, input.worldPosition, toEye, gPointLights[i], gMaterial.shininess);
            }
        
          [loop]
            for (int i = 0; i < 20; ++i)
            {
                lightTotalSpecular += CalculateSpotLightSpecular(normalInput, toEye, gSpotLights[i], gMaterial.shininess);
            }
            
        
            //directionalLightReflect
            lightTotalSpecular +=
            DirectionalLightDiffuse *
            CalculateDirectionalSpecular(normalInput, gDirectionalLight.direction, toEye, gDirectionalLight.color.rgb, gMaterial.shininess);
         
            output.color.rgb =
            baseColor * (DirectionalLightDiffuse + lightTotalDiffuse) + lightTotalSpecular;
            
        }
        else
        {
         //NoneReflect
            output.color.rgb = baseColor * (DirectionalLightDiffuse + lightTotalDiffuse);

        }
        
        //EnvironmentReflection    
        // ======================================================================
        float3 cameraToPosition = -toEye;
        float3 reflectedVector = reflect(cameraToPosition, normalize(input.normal));
        float4 environmentColor = gEnvironmentTexture.Sample(gSampler, reflectedVector);
        output.color.rgb += environmentColor.rgb * gMaterial.environmentCoefficient;
        // ======================================================================
        
        //commonLightMode
        output.color.a = gMaterial.color.a * textureColor.a;
    }

    //output.color discard
    if (output.color.a == 0.0)
    {
        discard;
    }
      
    return output;
}
