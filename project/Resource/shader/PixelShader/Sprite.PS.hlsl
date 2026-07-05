#include "../Hlsli/Sprite.hlsli"

//テクスチャを貼り付けたり、ライティングを行ったりと、もっとも主要なShaderである
struct Material
{
    float4 color;
    float32_t4x4 uvTransform;
    float temperature;
};

//ConstantBufferを定義する
//ConstantBuffer<構造体>変数名 : register(b0);//配置場所
//CPUから値を渡すにはConstantBufferという機能を利用する
ConstantBuffer<Material> gMaterial : register(b0);
Texture2D<float4> gTexture : register(t2); //SRVはt
SamplerState gSampler : register(s0); //Samplerはs これを介してtextureを読む

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
    float4 temperature : SV_TARGET1; //AddTemperature
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    float outlineMask = 1.0f - gMaterial.color.a;
    outlineMask = clamp(outlineMask, 0.0f, 1.0f);
    //SetTemperature　Use G Channel for OutlineMask
    output.temperature = float4(gMaterial.temperature, outlineMask, 0.0, 1.0);
    
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    
     //textureのα値ガ0.0以下の時にpixleを棄却
    if (textureColor.a <= 0.2)
    {
        discard;
    }
    
    output.color = gMaterial.color * textureColor; //ベクトル*ベクトルと記述すると乗算が行われる

    if (output.color.a <= 0.0)
    {
        discard;
    }
    
    
    return output;
}
