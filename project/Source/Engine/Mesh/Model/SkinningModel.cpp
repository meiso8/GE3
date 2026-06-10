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
    //スケルトンやクラスターを作成する
    *skeleton_ = Bone::CreateSkeleton(modelData_->rootNode);
    *skinCluster_ = CreateSkinCluster(*skeleton_, *modelData_);
}

void SkinningModel::PreDraw(ID3D12GraphicsCommandList* commandList, const BlendMode& blendMode, const CullMode& cullMode,  const MaskMode maskMode, const bool usePSOKey)
{
    (void)maskMode;
    (void)usePSOKey;

    commandList->SetGraphicsRootSignature(PSO::GetRootSignature()->GetRootSignature(RootSignature::SKINNING));
    commandList->SetPipelineState(PSO::GetGraphicsPipelineStatesSkinning(blendMode, cullMode).Get());//PSOを設定
    //形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておけばよい。
    commandList->IASetPrimitiveTopology(topology_);
}

void SkinningModel::Draw(ID3D12GraphicsCommandList* commandList)
{
    D3D12_VERTEX_BUFFER_VIEW vbvs[2] = { vertexBufferView_,skinCluster_->influenceBufferView };

    commandList->IASetVertexBuffers(0, 2, vbvs);//VBVを設定

    //IBVを設定
    commandList->IASetIndexBuffer(&indexBufferView_);//IBVを設定
    //cameraのCBufferの場所を設定 paletteResource 
    SrvManager::SetGraphicsRootDescriptorTable(11, skinCluster_->paletteSrvIndex);

    // 2. マテリアルごとにループを回して描画
    for (const auto& section : modelData_->sections) {

        // セクション名に対応するマテリアルデータを取得
        const auto& material = modelData_->materials.at(section.materialName);

        // 1. このセクション用のディフューズテクスチャのSRVインデックスを取得
        int32_t srvIndex = material.textureData_[TEXTURE_USAGE_DIFFUSE].textureSrvIndex;
        if (srvIndex == -1) {
            // テクスチャが無い場合は白1x1テクスチャ
            srvIndex = Texture::GetSRVHandle(TextureFactory::WHITE_1X1);
        }
        //拡散反射テクスチャ
        SrvManager::SetGraphicsRootDescriptorTable(2, srvIndex);
        // 3. インデックスの開始位置と個数を指定して描画コールを呼ぶ
        commandList->DrawIndexedInstanced(
            section.indexCount,  // 描画するインデックス数
            1,
            section.indexStart,  // ★開始インデックス位置（ここが重要！）
            0,
            0
        );

    }

    ////描画!（DrawCall/ドローコール）6個のインデックスを使用し1つのインスタンスを描画。その他は当面0で良い。
    //commandList->DrawIndexedInstanced(UINT(modelData_->indices.size()), 1, 0, 0, 0);
}

Matrix4x4* SkinningModel::GetJointMatrix(const std::string jointName)
{
    return  Bone::GetJointMatrix(jointName,*skeleton_);
}
