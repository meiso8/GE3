#include "object3d.hlsli"
#include"DirectionalLight.hlsli"

//テクスチャを貼り付けたり、ライティングを行ったりと、もっとも主要なShaderである


struct Material
{
    float32_t4 color;
    int32_t lightType;
    float32_t4x4 uvTransform;

};

//ConstantBufferを定義する
//ConstantBuffer<構造体>変数名 : register(b0);//配置場所
//CPUから値を渡すにはConstantBufferという機能を利用する
ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);

Texture2D<float32_t4> gTexture : register(t0); //SRVはt
SamplerState gSampler : register(s0); //Samplerはs これを介してtextureを読む

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};


PixelShaderOutput main(VertexShaderOutput input)
{
 
    float4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);

    PixelShaderOutput output;

    if (gMaterial.lightType == 0)
    {
        //Lightingしない場合。前回までと同じ演算
        output.color = gMaterial.color * textureColor; //ベクトル*ベクトルと記述すると乗算が行われる
     
    } else if (gMaterial.lightType == 1)
    {

        float cos = saturate(dot(normalize(input.normal), -gDirectionalLight.direction));
        //output.color = gMaterial.color * textureColor * gDirectionalLight.color * cos * gDirectionalLight.intensity;
        
        output.color.rgb = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
        output.color.a = gMaterial.color.a * textureColor.a;
    } else {
        //half lambert
        
        //法線とライトの方向の内積
        float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
         
        output.color.rgb = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
        output.color.a = gMaterial.color.a * textureColor.a;
    }
    
    return output;
}
