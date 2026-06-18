#include "LineMesh.h"
#include"DirectXCommon.h"

void LineMesh::Create(const Vector3& startPos, const Vector3& endPos)
{
    meshData_ = std::make_unique<MeshData>();
    *meshData_ = PrimitiveGenerator::CreateLine(startPos, endPos);

    Primitive::Create(*meshData_);

}
void LineMesh::SetVertex(const Vector3& start, const Vector3& end)
{

    meshData_->vertices[0].position = { start.x,start.y,start.z,1.0f };//左下
    meshData_->vertices[0].texcoord = { 0.0f,1.0f };
    meshData_->vertices[0].normal = { meshData_->vertices[0].position.x,    meshData_->vertices[0].position.y,     meshData_->vertices[0].position.z };//法線
    meshData_->vertices[1].position = { end.x,end.y,end.z,1.0f };//右下
    meshData_->vertices[1].texcoord = { 0.0f,0.0f };
    meshData_->vertices[1].normal = { meshData_->vertices[1].position.x,   meshData_->vertices[1].position.y,    meshData_->vertices[1].position.z };

    VertexData* vertexMap = nullptr;
    // 1. 頂点バッファの作成とデータ転送
    UINT vertexBufferSize = sizeof(VertexData) * vertexCount_;

    //頂点リソースにマッピング
    vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexMap));
    std::memcpy(vertexMap, meshData_->vertices.data(), vertexBufferSize); // vectorの中身を一気にコピー！
    vertexResource_->Unmap(0, nullptr);

};