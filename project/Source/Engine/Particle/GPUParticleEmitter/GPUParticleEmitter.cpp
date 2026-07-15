#include "GPUParticleEmitter.h"
#include"TimeManager.h"
#include"TransitionBarrier.h"
#include"ComputeShaderPSO/ComputeShaderPSO.h"
#include"PSO.h"
#include"CbvSrvUavDescriptorHeap.h"

CbvSrvUavDescriptorHeap* GPUParticleEmitter::cbvSrvUavDescriptorHeap_ = nullptr;
ID3D12GraphicsCommandList* GPUParticleEmitter::commandList_ = nullptr;

void GPUParticleEmitter::SetCommandListAndCbvSrvUavDescriptorHeap(ID3D12GraphicsCommandList* commandList, CbvSrvUavDescriptorHeap* cbvSrvUavDescriptorHeap)
{
    cbvSrvUavDescriptorHeap_ = cbvSrvUavDescriptorHeap;
    commandList_ = commandList;
}

void GPUParticleEmitter::Create()
{
    emitterResource_.CreateBufferResource(L"GPUEmitterResource");

    perFrameResource_.CreateBufferResource(L"PreFrameResource");

    Initialize();
}

void GPUParticleEmitter::Initialize()
{
    emitterResource_.Map();
    emitterResource_.data->count = 10;
    emitterResource_.data->frequency = 0.5f;
    emitterResource_.data->frequencyTime = 0.0f;
    emitterResource_.data->translate = Math::ZERO;
    emitterResource_.data->radius = 1.0f;
    emitterResource_.data->emit = 0;
    emitterResource_.UnMap();

    perFrameResource_.Map();
    perFrameResource_.data->deltaTime = TimeManager::DeltaTime();
    perFrameResource_.data->time = TimeManager::GameTime();
    perFrameResource_.UnMap();

}

void GPUParticleEmitter::Update()
{

    emitterResource_.Map();
    auto& emitterSphere = emitterResource_.data;
    emitterSphere->frequencyTime += TimeManager::DeltaTime();

    if (emitterSphere->frequency <= emitterSphere->frequencyTime) {
        emitterSphere->frequencyTime -= emitterSphere->frequency;
        emitterSphere->emit = 1;
    } else {
        emitterSphere->emit = 0;
    }

    emitterResource_.UnMap();

    perFrameResource_.Map();
    perFrameResource_.data->deltaTime = TimeManager::DeltaTime();
    perFrameResource_.data->time = TimeManager::GameTime();
    perFrameResource_.UnMap();


    if (commandList_ == nullptr) {
        return;
    }

    //グループがセットされていない！！
    if (particleGroup_ == nullptr) {
        return;
    }


    cbvSrvUavDescriptorHeap_->PreDraw(commandList_);

    commandList_->SetComputeRootSignature(PSO::GetRootSignature()->GetRootSignature(RootSignature::CS_EMIT_PARTICLE));
    commandList_->SetPipelineState(ComputeShaderPSO::GetInstance()->GetParticlePSO(ComputeShaderPSO::kParticleEmitPSO).Get());
    cbvSrvUavDescriptorHeap_->SetComputeRootDescriptorTable(0, particleGroup_->particleUAVResource_.uavIndex, commandList_);
    commandList_->SetComputeRootConstantBufferView(1, emitterResource_.GetGPUVirtualAddress());
    commandList_->SetComputeRootConstantBufferView(2, perFrameResource_.GetGPUVirtualAddress());
    cbvSrvUavDescriptorHeap_->SetComputeRootDescriptorTable(3, particleGroup_->particleFreeListIndexResource_.uavIndex, commandList_);
    cbvSrvUavDescriptorHeap_->SetComputeRootDescriptorTable(4, particleGroup_->particleFreeListResource_.uavIndex, commandList_);

    //ComputeShaderの実行
    commandList_->Dispatch(1, 1, 1);

    // ====================================================================
  // バリアを張る
  // ====================================================================
    D3D12_RESOURCE_BARRIER barrier[3];
    barrier[0].Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
    barrier[0].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier[0].UAV.pResource = particleGroup_->particleUAVResource_.Get();

    barrier[1].Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
    barrier[1].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier[1].UAV.pResource = particleGroup_->particleFreeListIndexResource_.Get();

    barrier[2].Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
    barrier[2].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier[2].UAV.pResource = particleGroup_->particleFreeListResource_.Get();
    commandList_->ResourceBarrier(3, barrier);


    commandList_->SetComputeRootSignature(PSO::GetRootSignature()->GetRootSignature(RootSignature::CS_UPDATE_PARTICLE));
    commandList_->SetPipelineState(ComputeShaderPSO::GetInstance()->GetParticlePSO(ComputeShaderPSO::kParticleUpdatePSO).Get());
    cbvSrvUavDescriptorHeap_->SetComputeRootDescriptorTable(0, particleGroup_->particleUAVResource_.uavIndex, commandList_);
    commandList_->SetComputeRootConstantBufferView(1, perFrameResource_.GetGPUVirtualAddress());
    cbvSrvUavDescriptorHeap_->SetComputeRootDescriptorTable(2, particleGroup_->particleFreeListIndexResource_.uavIndex, commandList_);
    cbvSrvUavDescriptorHeap_->SetComputeRootDescriptorTable(3, particleGroup_->particleFreeListResource_.uavIndex, commandList_);


    //ComputeShaderの実行
    commandList_->Dispatch(1, 1, 1);

}

void GPUParticleEmitter::SetParticleGroup(ParticleGroupGPU* particleGroup)
{
    particleGroup_ = particleGroup;
}
