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

    auto& emitterSphere = emitterResource_.data;
    emitterSphere->frequencyTime += TimeManager::DeltaTime();

    if (emitterSphere->frequency <= emitterSphere->frequencyTime) {
        emitterSphere->frequencyTime -= emitterSphere->frequency;
        emitterSphere->emit = 1;
    } else {
        emitterSphere->emit = 0;
    }

    perFrameResource_.Map();
    perFrameResource_.data->deltaTime = TimeManager::DeltaTime();
    perFrameResource_.data->time = TimeManager::GameTime();
    perFrameResource_.UnMap();

    TransitionBarrier tbarrir;
    tbarrir.SetCommandList(commandList_);

    if (commandList_ == nullptr) {
        return;
    }

    //グループがセットされていない！！
    if (particleGroup_ == nullptr) {
        return;
    }



    tbarrir.SettingBarrier(
        particleGroup_->particleUAVResource_.Get(),
        D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
        D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

    cbvSrvUavDescriptorHeap_->PreDraw(commandList_);

    commandList_->SetComputeRootSignature(PSO::GetRootSignature()->GetRootSignature(RootSignature::CS_EMIT_PARTICLE));
    commandList_->SetPipelineState(ComputeShaderPSO::GetInstance()->GetComputePipelineStatesForEmitParticle().Get());
    cbvSrvUavDescriptorHeap_->SetComputeRootDescriptorTable(0, particleGroup_->particleUAVResource_.uavIndex, commandList_);
    commandList_->SetComputeRootConstantBufferView(1, emitterResource_.GetGPUVirtualAddress());
    commandList_->SetComputeRootConstantBufferView(2, perFrameResource_.GetGPUVirtualAddress());
   
    //ComputeShaderの実行
    commandList_->Dispatch(1, 1, 1);

    tbarrir.SettingBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
}

void GPUParticleEmitter::SetParticleGroup(ParticleGroupGPU* particleGroup)
{
    particleGroup_ = particleGroup;
}
