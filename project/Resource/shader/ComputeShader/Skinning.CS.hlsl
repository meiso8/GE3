#include "../Hlsli/SkinningObject.hlsli"

struct Vertex
{
    float4 position;
    float2 texcoord;
    float3 normal;
};

struct VertexInfluence
{
    float4 weight;
    int4 index;
};

struct SkinningInformation
{
    uint numVertices;
};

//SkinningObject3d.VS.hlsl Same
StructuredBuffer<Well> gMatrixPalette : register(t6);
//VertexBufferView stream0 inputVertex
StructuredBuffer<Vertex> gInputVertices : register(t9);
//VertexBufferView stream1 inputInfluences
StructuredBuffer<VertexInfluence> gInfluences : register(t10);
//SkinningAfterCulVertex UAV
RWStructuredBuffer<Vertex> gOutputVertices : register(u0);
//SkiningInformation
ConstantBuffer<SkinningInformation> gSkinningInformation : register(b0);

[numthreads(1024, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint1 vertexIndex = DTid.x;
    if (vertexIndex < gSkinningInformation.numVertices)
    {
        Vertex input = gInputVertices[vertexIndex];
        VertexInfluence influence = gInfluences[vertexIndex];   
        
        Vertex skinned;
     
        skinned.position = mul(input.position, gMatrixPalette[influence.index.x].skeletonSpaceMatrix) * influence.weight.x;
        skinned.position += mul(input.position, gMatrixPalette[influence.index.y].skeletonSpaceMatrix) * influence.weight.y;
        skinned.position += mul(input.position, gMatrixPalette[influence.index.z].skeletonSpaceMatrix) * influence.weight.z;
        skinned.position += mul(input.position, gMatrixPalette[influence.index.w].skeletonSpaceMatrix) * influence.weight.w;
        skinned.position.w = 1.0f;
    

        skinned.normal = mul(input.normal, (float3x3) gMatrixPalette[influence.index.x].skeletonSpaceInverseTransposeMatrix) * influence.weight.x;
        skinned.normal += mul(input.normal, (float3x3) gMatrixPalette[influence.index.y].skeletonSpaceInverseTransposeMatrix) * influence.weight.y;
        skinned.normal += mul(input.normal, (float3x3) gMatrixPalette[influence.index.z].skeletonSpaceInverseTransposeMatrix) * influence.weight.z;
        skinned.normal += mul(input.normal, (float3x3) gMatrixPalette[influence.index.w].skeletonSpaceInverseTransposeMatrix) * influence.weight.w;
        skinned.normal = normalize(skinned.normal);

        skinned.texcoord = input.texcoord;
        
        //SkinningData
        gOutputVertices[vertexIndex] = skinned;
    }
    
}