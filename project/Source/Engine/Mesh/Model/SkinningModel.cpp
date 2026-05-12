#include "SkinningModel.h"
#include"SRVmanager/SrvManager.h"
#include"Model.h"
SkinningModel::SkinningModel()
{
    skeleton_ = std::make_unique<Skeleton>();
    skinCluster_ = std::make_unique<SkinCluster>();
}


void SkinningModel::SetModel(Model* model)
{
    //モデルのメッシュ
    modelData_ = model->GetModelData();
    vertexBufferView_ = model->GetVBV();
    indexBufferView_ = model->GetIBV();
    textureHandle_ = modelData_->material.textureSrvIndex;
}

void SkinningModel::SetBoneModel(Model* boneModel)
{
    //骨の情報
    boneData_ = boneModel->GetModelData();
    *skeleton_ = CreateSkeleton(boneData_->rootNode);
    *skinCluster_ = CreateSkinCluster(*skeleton_, *boneData_);
}

void SkinningModel::CreateDatas(Model* model, Model* boneModel)
{
    SetModel(model);
    SetBoneModel(boneModel);
}

void SkinningModel::PreDraw(ID3D12GraphicsCommandList* commandList, const BlendMode& blendMode, const CullMode& cullMode)
{
    commandList->SetGraphicsRootSignature(PSO::GetRootSignature()->GetRootSignature(RootSignature::SKINNING));
    commandList->SetPipelineState(PSO::GetGraphicsPipelineStatesSkinning(blendMode, cullMode).Get());//PSOを設定
    //形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておけばよい。
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void SkinningModel::Draw(ID3D12GraphicsCommandList* commandList)
{
    D3D12_VERTEX_BUFFER_VIEW vbvs[2] = { vertexBufferView_,skinCluster_->influenceBufferView };

    commandList->IASetVertexBuffers(0, 2, vbvs);//VBVを設定

    //IBVを設定
    commandList->IASetIndexBuffer(&indexBufferView_);//IBVを設定

    //SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である。
    SrvManager::SetGraphicsRootDescriptorTable(2, textureHandle_);
    //cameraのCBufferの場所を設定 paletteResource 
    SrvManager::SetGraphicsRootDescriptorTable(11, skinCluster_->paletteSrvIndex);

    //描画!（DrawCall/ドローコール）6個のインデックスを使用し1つのインスタンスを描画。その他は当面0で良い。
    commandList->DrawIndexedInstanced(UINT(modelData_->indices.size()), 1, 0, 0, 0);

}

void SkinningModel::ResetTextureHandle()
{
    textureHandle_ = modelData_->material.textureSrvIndex;
}
