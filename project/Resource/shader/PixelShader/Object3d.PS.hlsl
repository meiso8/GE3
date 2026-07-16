#include "../Hlsli/Object3d.hlsli"
#include"../HlSli/Object3dCommon.hlsli"
#include "../Hlsli/Camera.hlsli"
#include "../Hlsli/Light.hlsli"

struct ObjectMaterial
{
    float4 color;
    int32_t lightMode;
    float32_t shininess;
    float32_t environmentCoefficient;
    float temperature;
    float32_t4x4 uvTransform;
};

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
    float4 temperature : SV_TARGET1; //AddTemperature
    uint1 objectID : SV_TARGET2;
};

ConstantBuffer<ObjectMaterial> gMaterial : register(b0);
ConstantBuffer<Camera> gCamera : register(b2);
ConstantBuffer<ObjectID> gObjectID : register(b3);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b4);

SamplerState gSampler : register(s0);

Texture2D<float4> gTexture : register(t2);
StructuredBuffer<PointLight> gPointLights : register(t4);
StructuredBuffer<SpotLight> gSpotLights : register(t5);
TextureCube<float4> gEnvironmentTexture : register(t7);

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
   
    output.temperature = GetTemperatureTexture(gMaterial.color.a, gMaterial.temperature);
    output.objectID = gObjectID.id;
    
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
            lightTotalDiffuse += CalculateSpotLightDiffuse(normalInput, input.worldPosition, gSpotLights[i], gMaterial.lightMode);
        }
               
        float3 DirectionalLightDiffuse = CalculateDirectionalDiffuse(normalInput, gDirectionalLight.direction, gDirectionalLight.color.rgb, gDirectionalLight.intensity, gMaterial.lightMode);
        
        if (gMaterial.lightMode == 1)
        {
            
          [loop]
            for (int i = 0; i < 20; ++i)
            {
                lightTotalSpecular += CalculatePointLightSpecular(normalInput, input.worldPosition, toEye, gPointLights[i], gMaterial.shininess);
                lightTotalSpecular += CalculateSpotLightSpecular(normalInput, toEye, gSpotLights[i], gMaterial.shininess);
            }
                   
            //directionalLightReflect
            lightTotalSpecular +=
            DirectionalLightDiffuse *
            CalculateDirectionalSpecular(normalInput, gDirectionalLight.direction, toEye, gDirectionalLight.color.rgb, gMaterial.shininess);
         
            output.color.rgb =  baseColor * (DirectionalLightDiffuse + lightTotalDiffuse) + lightTotalSpecular;
            
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

    bool isColorAlpha = textureColor.a == 0.0 || gMaterial.color.a == 0.0;
    if (output.temperature.r <= 0.0 && isColorAlpha)
    {
        discard;
    }
   
    return output;
}
