#include "Model.h"
#include"SrvManager/SrvManager.h"

void Model::CreateModel()
{
    Create(*modelData_);
}

void Model::Draw(ID3D12GraphicsCommandList* commandList) {

    //形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておけばよい。
    commandList->IASetPrimitiveTopology(topology_);
    // 1. 頂点バッファ・インデックスバッファのセットは全体で1回だけ（外側で行う）
    commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
    commandList->IASetIndexBuffer(&indexBufferView_);

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
            section.indexStart,  // ★開始インデックス位置
            0,
            0
        );

    }
}
void Model::DrawCallForParticle(ID3D12GraphicsCommandList* commandList, const uint32_t numInstance)
{
    commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
    commandList->DrawInstanced(UINT(modelData_->vertices.size()), numInstance, 0, 0);
}
