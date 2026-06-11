#include "BeamObject3d.h"
#include"DirectXCommon.h"
#include"MakeMatrix.h"
#include"Lights/PointLightManager.h"
#include"Lights/DirectionalLightManager.h"
#include"Lights/SpotLightManager.h"
#include"SRVmanager/SrvManager.h"

void BeamObject3d::Create()
{

    Object3d::Create();
    CreatePointData();

}

void BeamObject3d::SetPos(const Vector3 start, const Vector3 end)
{
    pointData_->startPos = start;
    pointData_->endPos = end;
}

void BeamObject3d::SetStartPos(const Vector3 start)
{
    pointData_->startPos = start;
}

void BeamObject3d::SetEndPos(const Vector3 end)
{
    pointData_->endPos = end;
}

void BeamObject3d::Draw(Camera& camera, const BlendMode& blendMode, const CullMode& cullMode, const MaskMode maskMode, const bool usePSOKey, const TextureFactory::Handle skyBoxTexture)
{
    //データを書き込む
    transformationMatrixData_->World = worldTransform_.matWorld_;
    transformationMatrixData_->WorldInverseTranspose = Transpose(Inverse(worldTransform_.matWorld_));
    transformationMatrixData_->WVP = Multiply(worldTransform_.matWorld_, camera.GetViewProjectionMatrix());

    
    (void)usePSOKey;

    auto* commandlist = DirectXCommon::GetCommandList();

    if (meshCommon_) {

        commandlist->SetGraphicsRootSignature(PSO::GetRootSignature()->GetRootSignature(RootSignature::BEAM));

        //普通のPSO
        PSO::PSOKey key{};
        key.rootSignatureType = RootSignature::BEAM;
        key.vsShaderType = DxcCompiler::VS_Beam;
        key.psShaderType = DxcCompiler::PS_Normal;

        key.blendMode = blendMode;
        key.cullMode = cullMode;
        key.depthMode = maskMode;
        key.topologyType = PSO::kTriangle;
        key.inputLayoutType = InputLayout::kInputLayoutTypeNormal;
 
        auto pso = PSO::GetOrCreatePSO(key);
        commandlist->SetPipelineState(pso.Get());

        //マテリアルCBufferの場所を設定　/*RotParameter配列の0番目 0->register(b4)1->register(b0)2->register(b4)*/
        commandlist->SetGraphicsRootConstantBufferView(0, materialResource_->GetMaterialResource()->GetGPUVirtualAddress());
        //wvp用のCBufferの場所を設定
        commandlist->SetGraphicsRootConstantBufferView(1, transformationMatrixResource_->GetGPUVirtualAddress());

        SrvManager::SetGraphicsRootDescriptorTable(2, textureHandles_[TEXTURE_USAGE_DIFFUSE]);
        //ライトのCBufferの場所を設定 3
        DirectionalLightManager::SetGraphicsRootConstantBufferView(3);
        //pointのCBufferの場所を設定
        commandlist->SetGraphicsRootConstantBufferView(4, pointDataResource_->GetGPUVirtualAddress());

        //cameraのCBufferの場所を設定
        commandlist->SetGraphicsRootConstantBufferView(5, camera.GetResource()->GetGPUVirtualAddress());

        PointLightManager::SetGraphicsRootDescriptorTable(6);
        SpotLightManager::SetGraphicsRootDescriptorTable(7);
        SrvManager::SetGraphicsRootDescriptorTable(8, Texture::GetSRVHandle(skyBoxTexture));
        meshCommon_->Draw(commandlist);
    }

}

void BeamObject3d::CreatePointData()
{    

    //PointData
    pointDataResource_ = DirectXCommon::CreateBufferResource(sizeof(Point));
    //データを書き込む
    //書き込むためのアドレスを取得
    pointDataResource_->Map(0, nullptr, reinterpret_cast<void**>(&pointData_));
    pointData_->startPos = { 0.0f,0.0f,0.0f };
    pointData_->endPos = { 0.0f,0.0f,0.0f };

}
