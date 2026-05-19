#include "Object3d.h"
#include"DirectXCommon.h"
#include"MakeMatrix.h"
#include"Lights/PointLightManager.h"
#include"Lights/DirectionalLightManager.h"
#include"Lights/SpotLightManager.h"
#include"Model.h"
#include"SRVmanager/SrvManager.h"
ID3D12GraphicsCommandList* Object3d::commandList_ = nullptr;

void Object3d::CreateMaterial(const float temperature, const Vector4& color, const uint32_t& lightType) {
    //マテリアルリソースを作成
    materialResource_ = std::make_unique<MaterialResource>();
    materialResource_->CreateMaterial(temperature,color, lightType);
}

void Object3d::CreateUV()
{
    uvTransform_ = {
      {1.0f,1.0f,1.0f},
      {0.0f,0.0f,0.0f},
      {0.0f,0.0f,0.0f},
    };

    uvTransformMatrix_ = MakeIdentity4x4();

};

Object3d::~Object3d()
{
    if (waveResource_) {
        waveResource_->Unmap(0, nullptr);
    }

    if (expansionResource_) {
        expansionResource_->Unmap(0, nullptr);
    }

    waveResource_.Reset();
    expansionResource_.Reset();
}

void Object3d::InitBalloonData()
{
    //データを書き込む
    balloonData_->expansion = 0.0f;
    balloonData_->sphere = 0.0f;
    balloonData_->cube = 0.0f;
    balloonData_->isSphere = false;
}


void Object3d::InitWaveData()
{
    waveData_[0].direction = { 1.0f,0.0f,0.0f };
    waveData_[0].time = 0.0f;
    waveData_[0].amplitude = 0.0f;
    waveData_[0].frequency = 4;

    waveData_[1].direction = { 1.0f,0.0f,0.0f };
    waveData_[1].time = 0.0f;
    waveData_[1].amplitude = 0.0f;
    waveData_[1].frequency = 4;

}

void Object3d::InitWaveDataIndex(const uint32_t& index)
{
    if (index > 1) {
        return;
    }

    waveData_[index].direction = { 1.0f,0.0f,0.0f };
    waveData_[index].time = 0.0f;
    waveData_[index].amplitude = 0.0f;
    waveData_[index].frequency = 4;
}

void Object3d::CreateWaveData()
{
    int waveCount = 2;
    //waveResource_ = DirectXCommon::CreateBufferResource(sizeof(
    // ) * waveCount);
    size_t bufferSize = (sizeof(Wave) * waveCount + 255) & ~255;
    waveResource_ = DirectXCommon::CreateBufferResource(bufferSize);
    //書き込むためのアドレスを取得
    waveResource_->Map(0, nullptr, reinterpret_cast<void**>(&waveData_));

    InitWaveData();
}

void Object3d::CreateBalloonData()
{
    expansionResource_ = DirectXCommon::CreateBufferResource(sizeof(Balloon));

    //書き込むためのアドレスを取得
    expansionResource_->Map(0, nullptr, reinterpret_cast<void**>(&balloonData_));
    //データを初期化する
    InitBalloonData();

}

void Object3d::UpdateUV() {

    uvTransformMatrix_ = MakeAffineMatrix(uvTransform_.scale, uvTransform_.rotate, uvTransform_.translate);
    materialResource_->SetUV(uvTransformMatrix_);
}


void Object3d::Draw(Camera& camera, const BlendMode& blendMode, const CullMode& cullMode, const TextureFactory::Handle skyBoxTexture)
{
    //データを書き込む
    transformationMatrixData_->World = worldTransform_.matWorld_;
    transformationMatrixData_->WorldInverseTranspose = Transpose(Inverse(worldTransform_.matWorld_));
    transformationMatrixData_->WVP = Multiply(worldTransform_.matWorld_, camera.GetViewProjectionMatrix());

    if (meshCommon_) {
        meshCommon_->PreDraw(commandList_, blendMode, cullMode);
        //マテリアルCBufferの場所を設定　/*RotParameter配列の0番目 0->register(b4)1->register(b0)2->register(b4)*/
        commandList_->SetGraphicsRootConstantBufferView(0, materialResource_->GetMaterialResource()->GetGPUVirtualAddress());
        //wvp用のCBufferの場所を設定
        commandList_->SetGraphicsRootConstantBufferView(1, transformationMatrixResource_->GetGPUVirtualAddress());
        //timeのSRVの場所を設定
        commandList_->SetGraphicsRootShaderResourceView(4, waveResource_->GetGPUVirtualAddress());
        //expansionのCBufferの場所を設定
        commandList_->SetGraphicsRootConstantBufferView(5, expansionResource_->GetGPUVirtualAddress());
        //cameraのCBufferの場所を設定
        commandList_->SetGraphicsRootConstantBufferView(6, camera.GetResource()->GetGPUVirtualAddress());
        //ライトのCBufferの場所を設定
        DirectionalLightManager::SetGraphicsRootConstantBufferView();
        PointLightManager::SetGraphicsRootDescriptorTable();
        SpotLightManager::SetGraphicsRootDescriptorTable();
        SrvManager::SetGraphicsRootDescriptorTable(10, Texture::GetHandle(skyBoxTexture));
        meshCommon_->Draw(commandList_);
    }
}

void Object3d::DrawForEffect(Camera& camera, const BlendMode& blendMode, const TextureFactory::Handle skyBoxTexture)
{
    SetLightMode(kLightModeNone);
    //データを書き込む
    transformationMatrixData_->World = worldTransform_.matWorld_;
    transformationMatrixData_->WorldInverseTranspose = Transpose(Inverse(worldTransform_.matWorld_));
    transformationMatrixData_->WVP = Multiply(worldTransform_.matWorld_, camera.GetViewProjectionMatrix());

    if (meshCommon_) {
        commandList_->SetGraphicsRootSignature(PSO::GetRootSignature()->GetRootSignature(RootSignature::NORMAL));
        
        PSO::PSOKey key{};
        key.rootSignatureType = RootSignature::NORMAL;
        key.vsShaderType = DxcCompiler::VS_Normal;
        key.psShaderType = DxcCompiler::PS_Normal;
        key.blendMode = blendMode;
        key.cullMode = kCullModeNone;
        key.depthMode = kZero; // 追加した設定
        key.topologyType = PSO::kTriangle;
        key.inputLayoutType = InputLayout::kInputLayoutTypeNormal;

        auto pso = PSO::GetOrCreatePSO(key);
        commandList_->SetPipelineState(pso.Get());

        //マテリアルCBufferの場所を設定　/*RotParameter配列の0番目 0->register(b4)1->register(b0)2->register(b4)*/
        commandList_->SetGraphicsRootConstantBufferView(0, materialResource_->GetMaterialResource()->GetGPUVirtualAddress());
        //wvp用のCBufferの場所を設定
        commandList_->SetGraphicsRootConstantBufferView(1, transformationMatrixResource_->GetGPUVirtualAddress());
        //timeのSRVの場所を設定
        commandList_->SetGraphicsRootShaderResourceView(4, waveResource_->GetGPUVirtualAddress());
        //expansionのCBufferの場所を設定
        commandList_->SetGraphicsRootConstantBufferView(5, expansionResource_->GetGPUVirtualAddress());
        //cameraのCBufferの場所を設定
        commandList_->SetGraphicsRootConstantBufferView(6, camera.GetResource()->GetGPUVirtualAddress());
        //ライトのCBufferの場所を設定
        DirectionalLightManager::SetGraphicsRootConstantBufferView();
        PointLightManager::SetGraphicsRootDescriptorTable();
        SpotLightManager::SetGraphicsRootDescriptorTable();
        SrvManager::SetGraphicsRootDescriptorTable(10, Texture::GetHandle(skyBoxTexture));
        meshCommon_->Draw(commandList_);
    }


}

void Object3d::Create()
{
    commandList_ = DirectXCommon::GetCommandList();
    CreateTransformationMatrix();
    CreateMaterial();
    Initialize();
    CreateUV();
    CreateWaveData();
    CreateBalloonData();
}

void Object3d::Initialize()
{
    worldTransform_.Initialize();
}

void Object3d::Update()
{
    WorldTransformUpdate(worldTransform_);

}


void Object3d::CreateTransformationMatrix() {

    //Matrix4x4　1つ分のサイズを用意
    transformationMatrixResource_ = DirectXCommon::CreateBufferResource(sizeof(TransformationMatrixFor3D));
    //データを書き込む
    //書き込むためのアドレスを取得
    transformationMatrixResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData_));
    transformationMatrixData_->WVP = MakeIdentity4x4();
    transformationMatrixData_->World = MakeIdentity4x4();
    transformationMatrixData_->WorldInverseTranspose = MakeIdentity4x4();

}

