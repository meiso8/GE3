#include "../Hlsli/Object3d.hlsli"
 #include "../Hlsli/Camera.hlsli"

struct Material
{
    float4 color;
    int32_t lightType;
    float32_t4x4 uvTransform;

};


ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<Camera> gCamera : register(b2);
Texture2D<float4> gTexture : register(t2);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};


PixelShaderOutput main(VertexShaderOutput input)
{
 
    PixelShaderOutput output;
    
         //normal
    float3 normalInput = normalize(input.normal);

         //toCameraVector
    float3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
    
    float facingFactor = abs(dot(normalInput, toEye));
    
    facingFactor = pow(facingFactor, 2.0f);
    
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    output.color = gMaterial.color * textureColor * facingFactor;


    if (textureColor.a == 0.0)
    {
        discard;
    }
    
    return output;
}