#include "BeamObject3d.h"
#include"DirectXCommon.h"
#include"MakeMatrix.h"
#include"SRVmanager/SrvManager.h"
#include"Log.h"

void BeamObject3d::Draw(Camera& camera, const BlendMode& blendMode, const CullMode& cullMode, const MaskMode maskMode, const bool usePSOKey, const TextureFactory::Handle skyBoxTexture)
{

    (void)skyBoxTexture;

    //データを書き込む
    transformationMatrixData_->World = worldTransform_.matWorld_;
    transformationMatrixData_->WorldInverseTranspose = Transpose(Inverse(worldTransform_.matWorld_));
    transformationMatrixData_->WVP = Multiply(worldTransform_.matWorld_, camera.GetViewProjectionMatrix());

    if (primitive_) {


        primitive_->SetRootSignatureAndGraphicsPipeline(commandList_, blendMode, cullMode, maskMode, true, RootSignature::BEAM, DxcCompiler::VS_Beam, DxcCompiler::PS_Beam);
        //マテリアルCBufferの場所を設定　/*RotParameter配列の0番目 0->register(b4)1->register(b0)2->register(b4)*/
        commandList_->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
        //wvp用のCBufferの場所を設定
        commandList_->SetGraphicsRootConstantBufferView(1, transformationMatrixResource_->GetGPUVirtualAddress());
        SrvManager::SetGraphicsRootDescriptorTable(2, textureHandles_[TEXTURE_USAGE_DIFFUSE],commandList_);
        //cameraのCBufferの場所を設定
        commandList_->SetGraphicsRootConstantBufferView(3, camera.GetResource()->GetGPUVirtualAddress());
        //ID
        commandList_->SetGraphicsRootConstantBufferView(4, idResource_->GetGPUVirtualAddress());
        //メッシュデータの描画
        MeshDraw();
    }

}
