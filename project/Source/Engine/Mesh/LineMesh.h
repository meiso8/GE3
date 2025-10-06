#pragma once

#include"VertexData.h"
#include<wrl.h>
#include<d3d12.h>
#include"ModelConfig.h"

#include"commandList.h"  
#include"ModelData.h"  
#include"PSO.h"  
#include"ShaderResourceView.h"  
#include"Camera/Camera.h"  
#include"Transform.h"  
#include"TransformationMatrix.h"  
#include"MaterialResource.h"  
#include"Vector2.h"  
#include"RootSignature.h"  
#include"Balloon.h"
#include"Wave.h"
#include<d3d12.h>

class LineMesh
{
public:
    void Create(ModelConfig& mc,PSO& pso);

    void PreDraw();
    void Draw(
        ShaderResourceView& srv ,Camera& camera
    );

    void SetColor(const Vector4& color);
    void SetTranslate(const Vector3& translate) { transform_.translate = translate; }
    void SetRotate(const Vector3& rotate) { transform_.rotate = rotate; }
    void SetScale(const Vector3& scale) { transform_.scale = scale; }
    void SetVertexPos(const Vector3& start, const Vector3& end);


    Vector3& GetScaleRef() { return transform_.scale; };
    Vector3& GetRotateRef() { return transform_.rotate; };
    Vector3& GetTranslateRef() { return transform_.translate; };
    VertexData& GetVertexData(const uint32_t& index) {
        return vertexData_[index];
    }
    Material* GetMaterial() { return materialResource_.GetMaterial(); };
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

    MaterialResource materialResource_{};

    Microsoft::WRL::ComPtr<ID3D12Resource> expansionResource_;
    Balloon* expansionData_ = nullptr;

    Microsoft::WRL::ComPtr<ID3D12Resource> waveResource_;
    Wave* waveData = nullptr;

    ModelConfig modelConfig_{};
    PSO* pso_ = { nullptr };
};

