#pragma once
#include "ModelData.h"
#include"CommandList.h"
#include"WorldTransform.h"
#include"TransformationMatrix.h"
#include"MaterialResource.h"
#include"Transform.h"
#include"RootSignature.h"

#include<cstdint>
class Camera;
class ShaderResourceView;
class Particle
{

private:
    const uint32_t kNumInstance = 10;//インスタンス数
    TransformationMatrix* instancingData = nullptr;
    std::vector<Transform>transforms;

    ModelData modelData_;
    MaterialResource materialResource_{};

    CommandList* commandList_ = nullptr;
    RootSignature* rootSignature_ = nullptr;

    D3D12_CPU_DESCRIPTOR_HANDLE instancingSrvHandleCPU;
    D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandleGPU;
    Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource;


    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexBufferResource_;
    VertexData* vertexBufferData_ = nullptr;

public:
    void Create(const Microsoft::WRL::ComPtr<ID3D12Device>& device, CommandList& commandList,RootSignature& rootSignature);
    void Draw(Camera& camera, ShaderResourceView& srv);
private:
    void CreateModelData(
        const Microsoft::WRL::ComPtr<ID3D12Device>& device);
    void CreateTransformationMatrix(const Microsoft::WRL::ComPtr<ID3D12Device>& device);
};

