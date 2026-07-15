#include "GPUParticle.h"
#include"CbvSrvUavDescriptorHeap.h"
#include"MakeMatrix.h"
#include"PrimitiveFactory/PrimitiveFactory.h"
#include"Log.h"
#include"ComputeShaderPSO/ComputeShaderPSO.h"
#include"TransitionBarrier.h"

namespace {
    const uint32_t kMaxInstance_ = 1024;
}

void GPUParticleManager::Create(
    RootSignature* rootSignature,
    CbvSrvUavDescriptorHeap* cbvSrvUavDescriptorHeap,
    ID3D12GraphicsCommandList* commandList)
{

    rootSignature_ = rootSignature;
    cbvSrvUavDescriptorHeap_ = cbvSrvUavDescriptorHeap;
    commandList_ = commandList;

    CreateGroup();

}

void GPUParticleManager::Initialize()
{
    cbvSrvUavDescriptorHeap_->PreDraw(commandList_);

    commandList_->SetComputeRootSignature(PSO::GetRootSignature()->GetRootSignature(RootSignature::CS_INITIALIZE_PARTICLE));
    commandList_->SetPipelineState(ComputeShaderPSO::GetInstance()->GetParticlePSO(ComputeShaderPSO::kParticleInitializePSO).Get());
    cbvSrvUavDescriptorHeap_->SetComputeRootDescriptorTable(0, particleGroup_->particleUAVResource_.uavIndex, commandList_);
    cbvSrvUavDescriptorHeap_->SetComputeRootDescriptorTable(1, particleGroup_->particleFreeListIndexResource_.uavIndex, commandList_);
    cbvSrvUavDescriptorHeap_->SetComputeRootDescriptorTable(2, particleGroup_->particleFreeListResource_.uavIndex, commandList_);

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
}

void GPUParticleManager::CreateGroup()
{
    particleGroup_ = std::make_unique<ParticleGroupGPU>();

    UINT particleBufferSize = sizeof(ParticleCS) * kMaxInstance_;
    //===================//UAVResourceの作成//=======================

    particleGroup_->particleUAVResource_.CreateBufferResourceForUAV(L"GPU_UAV_ParticleResource", particleBufferSize);
    
    //UAVの作成をするよ
    particleGroup_->particleUAVResource_.AllocateUAV(cbvSrvUavDescriptorHeap_);
    particleGroup_->particleUAVResource_.CreateUAV(cbvSrvUavDescriptorHeap_, kMaxInstance_);

    //===================//SRVResourceの作成//=======================

    //SRVの作成をするよ
    particleGroup_->particleUAVResource_.AllocateSRV(cbvSrvUavDescriptorHeap_);
    particleGroup_->particleUAVResource_.CreateSRVforStructuredBuffer(cbvSrvUavDescriptorHeap_, kMaxInstance_);

    // =================//FreeListUAVの作成================================================
    //FreeList Index!!
    particleGroup_->particleFreeListIndexResource_.CreateBufferResourceForUAV(L"GPU_UAV_FreeList_Index_ParticleResource", particleBufferSize);
    particleGroup_->particleFreeListIndexResource_.AllocateUAV(cbvSrvUavDescriptorHeap_);
    particleGroup_->particleFreeListIndexResource_.CreateUAV(cbvSrvUavDescriptorHeap_, kMaxInstance_);
    //FreeList
    particleGroup_->particleFreeListResource_.CreateBufferResourceForUAV(L"GPU_UAV_FreeList_ParticleResource", particleBufferSize);
    particleGroup_->particleFreeListResource_.AllocateUAV(cbvSrvUavDescriptorHeap_);
    particleGroup_->particleFreeListResource_.CreateUAV(cbvSrvUavDescriptorHeap_, kMaxInstance_);

    //マテリアル用のリソースを作る。
    particleGroup_->materialResource.CreateBufferResource(L"GPU ParticleGroup_MaterialResource\n");

    //書き込むためのアドレスを取得
    HRESULT result = particleGroup_->materialResource.Map();
    particleGroup_->materialResource.data->color = { 1.0f,1.0f,1.0f,1.0f };
    particleGroup_->materialResource.data->lightMode = Object3d::LightMode::kLightModeNone;
    particleGroup_->materialResource.data->uvTransform = MakeIdentity4x4();
    particleGroup_->materialResource.data->shininess = 50.0f;
    particleGroup_->materialResource.data->environmentCoefficient = 0.0f;
    //体温 
    particleGroup_->materialResource.data->temperature = 1.0f;
    //テクスチャ
    particleGroup_->materialData.textureData_[TEXTURE_USAGE_DIFFUSE].textureSrvIndex = Texture::GetSRVHandle(TextureFactory::CIRCLE);

    //プリミティブメッシュの作成
    particleGroup_->primitive = std::make_unique<Primitive>();
    assert(particleGroup_->primitive);

    MeshData meshData = PrimitiveFactory::GetMeshData(Primitive::kPlane);
    particleGroup_->primitive->Create(meshData);

    //パーティクル用のViewを作成する
    particleGroup_->parViewResource.CreateBufferResource(L"Particle_InstancingResource");
    //書き込むためのアドレスを取得
    particleGroup_->parViewResource.Map();
    particleGroup_->parViewResource.data->billboardMatrix = MakeIdentity4x4();
    particleGroup_->parViewResource.data->viewProjection = MakeIdentity4x4();
    particleGroup_->parViewResource.UnMap();

    LogFile::Log("Create Particle Group\n");
}

void GPUParticleManager::Draw(Camera& camera)
{
    //マトリックスの更新
    particleGroup_->parViewResource.Map();
    Matrix4x4 billboardMatrix = Math::GetBillBordMatrix(camera.GetWorldMatrix());
    particleGroup_->parViewResource.data->billboardMatrix = billboardMatrix;
    particleGroup_->parViewResource.data->viewProjection = camera.GetViewProjectionMatrix();
    particleGroup_->parViewResource.UnMap();

#pragma region //Draw
    //rootSignatureの設定
    commandList_->SetGraphicsRootSignature(rootSignature_->GetRootSignature(RootSignature::PARTICLE_GPU));
    commandList_->SetPipelineState(PSO::graphicsPipelineStatesGPUParticle().Get());

    //マテリアルの設定   ConstantBuffer<Material> gMaterial : register(b0);
    commandList_->SetGraphicsRootConstantBufferView(0, particleGroup_->materialResource.GetGPUVirtualAddress());

    //テスクチャ Texture2D<float4> gTexture : register(t2);
    cbvSrvUavDescriptorHeap_->SetGraphicsRootDescriptorTable(1, particleGroup_->materialData.textureData_[TEXTURE_USAGE_DIFFUSE].textureSrvIndex, commandList_);

    // StructuredBuffer<Particle> gParticles : register(t11);
    cbvSrvUavDescriptorHeap_->SetGraphicsRootDescriptorTable(2, particleGroup_->particleUAVResource_.srvIndex, commandList_);
    // ConstantBuffer<ParView> gParView : register(b0);
    commandList_->SetGraphicsRootConstantBufferView(3, particleGroup_->parViewResource.GetGPUVirtualAddress());
   
    auto& primitive = particleGroup_->primitive;
    //形状設定
    commandList_->IASetPrimitiveTopology(primitive->GetTopology());
    //頂点データ
    commandList_->IASetVertexBuffers(0, 1, &primitive->GetVertexBufferView());
    //インデックスデータ
    commandList_->IASetIndexBuffer(&primitive->GetIndexBufferView());
    //描画!（DrawCall/ドローコール）6個のインデックスを使用しインスタンスを描画。
    commandList_->DrawIndexedInstanced(primitive->GetIndexCount(), kMaxInstance_, 0, 0, 0);

#pragma endregion

}

