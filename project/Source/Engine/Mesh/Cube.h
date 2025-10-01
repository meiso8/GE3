#pragma once


#include"VertexData.h"
#include<wrl.h>
#include<d3d12.h>
#include"Config.h"

#include"CommandList.h"  
#include"ModelData.h"  
#include"PSO.h"  
#include"ShaderResourceView.h"  
#include"Camera/Camera.h"  
#include"TransformationMatrix.h"  
#include"MaterialResource.h"  
#include"Vector2.h"  
#include"RootSignature.h"  
#include"Balloon.h"
#include"Wave.h"
#include"WorldTransform.h"

class Cube
{


public:
    void Create(
        const Microsoft::WRL::ComPtr<ID3D12Device>& device, ModelConfig& mc);
    void  PreDraw();
    void Draw(
        ShaderResourceView& srv, Camera& camera, const Matrix4x4& worldMatrix, uint32_t lightType = MaterialResource::LIGHTTYPE::NONE
    );

    void SetColor(const Vector4& color);
    void SetMinMax(const Vector3& min, const Vector3& max);

    VertexData& GetVertexData(const uint32_t& index) {
        return vertexData_[index];
    }
    Material* GetMaterial() { return materialResource_.GetMaterial(); };
private:
    void CreateVertex(const Microsoft::WRL::ComPtr<ID3D12Device>& device);
    void CreateIndexResource(const Microsoft::WRL::ComPtr<ID3D12Device>& device);
    void CreateTransformationMatrix(const Microsoft::WRL::ComPtr<ID3D12Device>& device);
    void CreateMaterial(const Microsoft::WRL::ComPtr<ID3D12Device>& device);
private:
    Microsoft::WRL::ComPtr <ID3D12Resource> vertexResource_{};
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
    VertexData* vertexData_ = nullptr;
    D3D12_INDEX_BUFFER_VIEW  indexBufferView_{};
    Microsoft::WRL::ComPtr <ID3D12Resource> indexResource_{};
    uint32_t* indexData_ = nullptr;

    Microsoft::WRL::ComPtr <ID3D12Resource> transformationMatrixResource_ = nullptr;

    Matrix4x4 worldViewProjectionMatrix_{};
    TransformationMatrix* transformationMatrixData_ = nullptr;

    MaterialResource materialResource_{};

    ModelConfig modelConfig_{};

    Microsoft::WRL::ComPtr<ID3D12Resource> expansionResource_;
    Balloon* expansionData_ = nullptr;

    Microsoft::WRL::ComPtr<ID3D12Resource> waveResource_;
    Wave* waveData_ = nullptr;
};

