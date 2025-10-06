#pragma once  

#include"CommandList.h"  
#include"ModelData.h"  
#include"PSO.h"  
#include"ShaderResourceView.h"  
#include"Camera/Camera.h"  
#include"Transform.h"  
#include"TransformationMatrix.h"  
#include"MaterialResource.h"  
#include"Vector2.h"  
#include"RootSignature.h"  
#include"ModelConfig.h"
#include"Balloon.h"
#include"Wave.h"

class Sprite
{
public:
    void Create( ModelConfig& mc);
    void UpdateUV();

    void PreDraw(PSO& pso);
    void Draw(
        ShaderResourceView& srv, Camera& camera, uint32_t lightType = MaterialResource::LIGHTTYPE::NONE
    );

    void SetSize(const Vector2& size);
    void SetColor(const Vector4& color);
    void SetTranslate(const Vector3& translate) { transform_.translate = translate; }

    Vector2& GetSize() { return size_; }

    Vector3& GetScaleRef() { return transform_.scale; };
    Vector3& GetRotateRef() { return transform_.rotate; };
    Vector3& GetTranslateRef() { return transform_.translate; };

    Material* GetMaterial() { return materialResource_.GetMaterial(); };

    Vector3& GetUVScale() { return uvTransform_.scale; };
    Vector3& GetUVRotate() { return uvTransform_.rotate; };
    Vector3& GetUVTranslate() { return uvTransform_.translate; };

private:
    void CreateVertex();
    void CreateIndexResource();
    void CreateTransformationMatrix();
    void CreateMaterial();
private:
    Microsoft::WRL::ComPtr <ID3D12Resource> vertexResource_{};
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
    VertexData* vertexData_ = nullptr;
    D3D12_INDEX_BUFFER_VIEW  indexBufferView_{};
    Microsoft::WRL::ComPtr <ID3D12Resource> indexResource_{};
    uint32_t* indexData_ = nullptr;

    Microsoft::WRL::ComPtr <ID3D12Resource> transformationMatrixResource_ = nullptr;

    Transform transform_{};
    Matrix4x4 worldMatrix_{};
    Matrix4x4 worldViewProjectionMatrix_{};
    TransformationMatrix* transformationMatrixData_ = nullptr;

    Transform uvTransform_ = { 0.0f };
    Matrix4x4 uvTransformMatrix_{};

    MaterialResource materialResource_{};

    Microsoft::WRL::ComPtr<ID3D12Resource> expansionResource_;
    Balloon* expansionData_ = nullptr;

    Microsoft::WRL::ComPtr<ID3D12Resource> waveResource_;
    Wave* waveData = nullptr;

    ModelConfig modelConfig_{};

    Vector2 size_;
};