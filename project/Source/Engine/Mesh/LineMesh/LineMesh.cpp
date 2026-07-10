#include "LineMesh.h"

LineMesh::LineMesh()
{
}

LineMesh::~LineMesh()
{

}

void LineMesh::Create(const Vector3& startPos, const Vector3& endPos)
{

    Primitive::Create(PrimitiveGenerator::CreateLine(startPos, endPos));

}
void LineMesh::SetVertex(const Vector3& start, const Vector3& end)
{
    //頂点リソースにマッピング
    vertexResource_.Map();
    vertexResource_.data[0].position = { start.x,start.y,start.z,1.0f };//左下
    vertexResource_.data[0].texcoord = { 0.0f,1.0f };
    vertexResource_.data[0].normal = { vertexResource_.data[0].position.x,   vertexResource_.data[0].position.y,    vertexResource_.data[0].position.z };//法線
    vertexResource_.data[1].position = { end.x,end.y,end.z,1.0f };//右下
    vertexResource_.data[1].texcoord = { 0.0f,0.0f };
    vertexResource_.data[1].normal = { vertexResource_.data[1].position.x,  vertexResource_.data[1].position.y,   vertexResource_.data[1].position.z };

    vertexResource_.UnMap();

};