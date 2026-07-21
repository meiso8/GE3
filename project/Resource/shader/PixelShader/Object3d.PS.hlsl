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
    
    float32_t maskVal;
    float32_t3 rgb;
    
    
    float maskEdgeMin;
    float maskEdgeMax;
    float maskGamma;
    float padding;
    
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

Texture2D<float> gMaskTexture : register(t0);
Texture2D<float4> gTexture : register(t2);
StructuredBuffer<PointLight> gPointLights : register(t4);
StructuredBuffer<SpotLight> gSpotLights : register(t5);
TextureCube<float4> gEnvironmentTexture : register(t7);

float3 Dissolve(float edgeMin,float edgeMax,float mask,float3 edgeColor,float gamma)
{
    
    float edge = 1.0f - smoothstep(edgeMin, edgeMax, mask);
    return edge * edgeColor * gamma;
}


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
    
    //Mask
    float mask = gMaskTexture.Sample(gSampler, input.texcoord);
    
    if (mask <= gMaterial.maskVal)
    {
        discard;
    }
    
    
    if (gMaterial.lightMode == 0)
    {
        output.color = gMaterial.color * textureColor;
        return output;
    }
    //toCameraVector
    float3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
    
    output.color = GetCalculateAllLightColor(
    gMaterial.lightMode,
    gMaterial.shininess,
    gMaterial.color,
    textureColor,
    input.normal,
    input.worldPosition,
    toEye,
    gPointLights,
    gSpotLights,
    gDirectionalLight
    );
    
    if (gMaterial.lightMode != 0)
    {
        //EnvironmentReflection    
        // ======================================================================
        float3 cameraToPosition = -toEye;
        float3 reflectedVector = reflect(cameraToPosition, normalize(input.normal));
        float4 environmentColor = gEnvironmentTexture.Sample(gSampler, reflectedVector);
        output.color.rgb += environmentColor.rgb * gMaterial.environmentCoefficient;
        // ======================================================================
    }
    
    if (input.meltTime >= 0.0f)
    {
        output.color.rgb += Dissolve(0.0f, input.meltTime, mask, gMaterial.rgb, 4.0f);
    }
    else
    {
        output.color.rgb += Dissolve(gMaterial.maskEdgeMin, gMaterial.maskEdgeMax, mask, gMaterial.rgb, gMaterial.maskGamma);
    }
      

    
    if (DisCardColor(textureColor.a, gMaterial.color.a, output.temperature.r))
    {
        discard;
    }
    
    return output;
}
