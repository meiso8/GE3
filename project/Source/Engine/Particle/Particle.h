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
public:
    void Create(const Microsoft::WRL::ComPtr<ID3D12Device>& device, CommandList& commandList,RootSignature& rootSignature);
    void Draw(Camera& camera);
private:
    void CreateModelData(
        const Microsoft::WRL::ComPtr<ID3D12Device>& device);
    void CreateTransformationMatrix(const Microsoft::WRL::ComPtr<ID3D12Device>& device);
};

