#pragma once


#include"Texture.h"
#include"Camera/Camera.h"
#include"ModelData.h"
#include"MaterialResource.h"
#include"TransformationMatrix.h"
#include"ModelConfig.h"
#include"Balloon.h"
#include"Wave.h"
#include<d3d12.h>

class SphereMesh
{
public:

    SphereMesh(ModelConfig mc)
        : modelConfig_(mc)
    {
    }

    void Create(ModelConfig& mc);

    void PreDraw(PSO& pso,PSO::PSOType type = PSO::PSOType::TRIANGLE);
    void Draw(Camera& camera, ShaderResourceView& srv,uint32_t lightType);

    Material* GetMaterial() { return materialResource_.GetMaterial(); }

    VertexData* GetVertexData() { return vertexData_; }
    Transform& GetUVTransform() { return uvTransform_; }
    Vector4& GetColor() { return materialResource_.GetMaterial()->color; }
    void SetColor(const Vector4& color);
    void SetUVScale(const Vector3& scale) { uvTransform_.scale = scale; };
    void UpdateUV();
    Balloon& GetExpansionData() {
        return *expansionData_;
    }
    Vector3& GetScale() { return transform_.scale; };
    Vector3& GetRotate() { return transform_.rotate; };
    Vector3& GetTranslate() { return transform_.translate; };

    ~SphereMesh();

private:
    void CreateVertex();
    void CreateWorldVPResource();
    void CreateMaterial();
private:
    ShaderResourceView srv_;

    Camera* camera_ = nullptr;

    Transform transform_{};
    Matrix4x4 worldMatrix_{};

    Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_;
    TransformationMatrix* wvpDate_ = nullptr;

    ModelConfig modelConfig_;

    Matrix4x4 worldViewProjectionMatrix_ = { 0.0f };
    MaterialResource materialResource_;

    Microsoft::WRL::ComPtr<ID3D12Resource> expansionResource_;
    Balloon* expansionData_ = nullptr;

    Microsoft::WRL::ComPtr<ID3D12Resource> waveResource_;
    Wave* waveData_ = nullptr;


    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
    VertexData* vertexData_ = nullptr;

    Transform uvTransform_ = { 0.0f };
    Matrix4x4 uvTransformMatrix_{};

    Texture* texture_ = nullptr;

    const uint32_t kSubdivision_ = 16;//分割数

};


