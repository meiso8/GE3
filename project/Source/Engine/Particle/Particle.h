#pragma once
#include "ModelData.h"
#include"commandList.h"
#include"WorldTransform.h"
#include"TransformationMatrix.h"
#include"MaterialResource.h"
#include"Transform.h"
#include"RootSignature.h"
#include "BlendState.h"

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
    RootSignature* rootSignature_ = nullptr;

    D3D12_CPU_DESCRIPTOR_HANDLE instancingSrvHandleCPU;
    D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandleGPU;
    Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource;

    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexBufferResource_;
    VertexData* vertexBufferData_ = nullptr;

public:
    void Create();
    void Draw(Camera& camera, ShaderResourceView& srv, BlendMode blendMode = BlendMode::kBlendModeNormal);
private:
    void CreateModelData();
    void CreateTransformationMatrix();
};

