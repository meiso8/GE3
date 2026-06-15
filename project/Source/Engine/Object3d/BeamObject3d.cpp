#include "BeamObject3d.h"
#include"DirectXCommon.h"
#include"MakeMatrix.h"
#include"Lights/PointLightManager.h"
#include"Lights/DirectionalLightManager.h"
#include"Lights/SpotLightManager.h"
#include"SRVmanager/SrvManager.h"

void BeamObject3d::Draw(Camera& camera, const BlendMode& blendMode, const CullMode& cullMode, const MaskMode maskMode, const bool usePSOKey, const TextureFactory::Handle skyBoxTexture)
{

    (void)skyBoxTexture;

    //データを書き込む
    transformationMatrixData_->World = worldTransform_.matWorld_;
    transformationMatrixData_->WorldInverseTranspose = Transpose(Inverse(worldTransform_.matWorld_));
    transformationMatrixData_->WVP = Multiply(worldTransform_.matWorld_, camera.GetViewProjectionMatrix());

    auto* commandlist = DirectXCommon::GetCommandList();

    if (meshCommon_) {
        meshCommon_->PreDraw(commandlist, blendMode, cullMode, maskMode, usePSOKey, RootSignature::BEAM, DxcCompiler::VS_Beam, DxcCompiler::PS_Beam);
        //マテリアルCBufferの場所を設定　/*RotParameter配列の0番目 0->register(b4)1->register(b0)2->register(b4)*/
        commandlist->SetGraphicsRootConstantBufferView(0, materialResource_->GetMaterialResource()->GetGPUVirtualAddress());
        //wvp用のCBufferの場所を設定
        commandlist->SetGraphicsRootConstantBufferView(1, transformationMatrixResource_->GetGPUVirtualAddress());
        SrvManager::SetGraphicsRootDescriptorTable(2, textureHandles_[TEXTURE_USAGE_DIFFUSE]);
        //cameraのCBufferの場所を設定
        commandlist->SetGraphicsRootConstantBufferView(3, camera.GetResource()->GetGPUVirtualAddress());
        meshCommon_->Draw(commandlist);
    }

}
