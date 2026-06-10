#pragma once
#include"VertexData.h"
#include<wrl.h>
#include<d3d12.h>
#include"PSO.h"  
#include"Texture.h"
#include"AABB.h"
#include"Sphere.h"
#include"ModelData.h"
#include"Circle.h"

class PrimitiveGenerator {
public:
    static MeshData CreateCube(const AABB& aabb = {.min = {-0.5f,-0.5f,-0.5f},.max = {0.5f,0.5f,0.5f}});
    static MeshData CreateSkyBox(const AABB& aabb = { .min = {-0.5f,-0.5f,-0.5f},.max = {0.5f,0.5f,0.5f} });
    static MeshData CreateLine(const Vector3& start, const Vector3& end);
    static MeshData CreateCircle(const Circle& circle = { .center = {0.0f,0.0f,0.0f },.radius = 4.0f }, const uint32_t kSubdivision = 16);
    static MeshData CreatePlane(const Vector2& size);
    static MeshData CreateRing(const float innerRadius = 0.2f, const float outerRadius = 1.0f, const uint32_t kRingDivide = 32);
    static MeshData CreateSphere(const Sphere& sphere = { .center = {0.0f,0.0f,0.0f },.radius = 0.5f }, const uint32_t kSubdivision = 16);
    static MeshData CreateCylinder(const bool isFlip = false, const float topRadius = 1.0f, const float bottomRadius = 1.0f, const float height = 3.0f, const uint32_t cylinderDivide = 32);
    static MeshData CreateBeam(const float firstSize = 0.1f);

};

class Primitive {

public:
    enum class MeshType {
        kNormal, // 三角形ポリゴン
        kLine    // ライン
    };

    virtual void Create(const MeshData& meshData);
    virtual void PreDraw(ID3D12GraphicsCommandList* commandList, const BlendMode& blendMode, const CullMode& cullMode);
    virtual void Draw(ID3D12GraphicsCommandList* commandList);  
    virtual void DrawCallForParticle(ID3D12GraphicsCommandList* commandList, const uint32_t numInstance);
protected:

    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
    D3D12_INDEX_BUFFER_VIEW  indexBufferView_{};
    D3D_PRIMITIVE_TOPOLOGY topology_ = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    Microsoft::WRL::ComPtr <ID3D12Resource> vertexResource_{};
    Microsoft::WRL::ComPtr <ID3D12Resource> indexResource_{};

    UINT vertexCount_ = 0;
private:


    MeshType meshType_ = MeshType::kNormal; // デフォルトはNormal
    UINT indexCount_ = 0;
};
