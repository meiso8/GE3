#include "object3d.hlsli"

//テクスチャを貼り付けたり、ライティングを行ったりと、もっとも主要なShaderである

struct Material
{
    float32_t4 color;
};

struct DoFParam
{
    float sigma;
    uint kernel;
    float focusDepth; //焦点距離を追加
};

//ConstantBufferを定義する
//ConstantBuffer<構造体>変数名 : register(b0);//配置場所
//CPUから値を渡すにはConstantBufferという機能を利用する
ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DoFParam> gDoFParam : register(b1);

Texture2D<float32_t4> gTexture : register(t0); //SRVはt
Texture2D<float32_t4> gDepthTexture : register(t1); //追加

SamplerState gSampler : register(s0); //Samplerはs これを介してtextureを読む


float GaussianWeight(float x, float sigma)
{
    float k = 1.0 / (sigma * sqrt(2.0 * 3.14159265359));
    return k * exp(-(x * x) / (2.0 * sigma * sigma));
}

float4 HorizontalBlur(Texture2D sceneTex, SamplerState sample, float2 uv, float2 offset, float sigma, uint kernel)
{
    float4 color = { 0.0f, 0.0f, 0.0f, 0.0f };
    

    for (int i = -int(kernel - 1 / 2); i <= int(kernel - 1 / 2); i++)
    {
        color +=
        sceneTex.Sample(sample, uv + float2(i * offset.x, 0)) * GaussianWeight(i, sigma);
    }
    
    return color;
}

float4 VerticalBlur(Texture2D sceneTex, SamplerState sample, float2 uv, float2 offset, float sigma, uint kernel)
{
    float4 color = { 0.0f, 0.0f, 0.0f, 0.0f };
     
    for (int i = -int(kernel - 1 / 2); i <= int(kernel - 1 / 2); i++)  // カーネルを拡大
    {
        color += sceneTex.Sample(
        sample, uv
        + float2(0, i * offset.y)) * GaussianWeight(i, sigma);
    }
    return color;
}

float4 GaussianBlur(Texture2D sceneTex, SamplerState sample, float2 uv, float sigma, uint kernel)
{
    float2 offset = float2(1.0 / 1280.0, 1.0 / 720.0);

    float4 color = { 0.0f, 0.0f, 0.0f, 0.0f };
    
    color = HorizontalBlur(sceneTex, sample, uv, offset, sigma, kernel);
    color += VerticalBlur(sceneTex, sample, uv, offset, sigma, kernel);

    return color;
}



struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};


PixelShaderOutput main(VertexShaderOutput input)
{
 
    PixelShaderOutput output;
    
    //深度値を取得　tex2Dとは2DTextureの特定の座標からデータを取得する関数？
    float depth = gDepthTexture.Sample(gSampler, input.texcoord).r; //深度値の情報はRGBのrだけに入っているのでrだけを取得
    
    //焦点を合わせる計算
    float blurFactor = abs(depth - gDoFParam.focusDepth);

    //オリジナルカラー
    float32_t4 originalColor = gTexture.Sample(gSampler, input.texcoord);
     //ガウスブラー処理をしたもの
    float32_t4 blurColor = GaussianBlur(gTexture, gSampler, input.texcoord, gDoFParam.sigma, gDoFParam.kernel);
    
    //線形補間しているというわけですね
    output.color = lerp(originalColor, blurColor, blurFactor);
    
    //output.color = blurColor;
    
    //output.color = gMaterial.color * lerp(originalColor, blurColor, blurFactor);
    
    
    return output;
}

