
#include "../Hlsli/Sprite.hlsli"

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    //行列の積を計算する関数がhlslの組み込み関数で定義されている
    output.position = mul(input.position, gTransformationMatrix.WVP);    
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float3x3) gTransformationMatrix.World));
    return output;
}