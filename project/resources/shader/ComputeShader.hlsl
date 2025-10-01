//[numthreads(1, 1, 1)]
//void main( uint3 DTid : SV_DispatchThreadID )
//{
//}

cbuffer ConstantBuffer : register(b0)
{
    float time; // アニメーション用の時間変数
};

RWStructuredBuffer<float3> vertices : register(u0); // 書き換え可能な頂点バッファ

[numthreads(256, 1, 1)] // スレッド数
void CSMain(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    float3 currentVertex = vertices[dispatchThreadID.x];

    // アニメーションのロジック（例: サイン波で頂点を移動）
    currentVertex.y += sin(time + dispatchThreadID.x) * 0.5f;

    vertices[dispatchThreadID.x] = currentVertex; // 書き戻し
}