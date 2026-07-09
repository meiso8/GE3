#include "SpriteCommon.h"
#include"DirectXCommon.h"
#include"PSO.h"
#include"CommandList.h"
#include"Log.h"

RootSignature* SpriteCommon::rootSignature_ = nullptr;

D3D12_INDEX_BUFFER_VIEW SpriteCommon::indexBufferView_;

 CResource<uint32_t> SpriteCommon::indexResource_;

void SpriteCommon::Finalize()
{
    indexResource_.Reset();
    LogFile::Log("Finalize SpriteCommon");
}

void SpriteCommon::Initialize(RootSignature* rootSignature)
{
    rootSignature_ = rootSignature;
    CreateIndexResource();
    LogFile::Log("Initialize SpriteCommon");
}

void SpriteCommon::SetIndexBuffer(ID3D12GraphicsCommandList* commandList)
{
    //IBVを設定new
    commandList->IASetIndexBuffer(&indexBufferView_);//IBVを設定

}

void SpriteCommon::PreDraw(uint32_t blendMode,ID3D12GraphicsCommandList* commandList)
{
    commandList->SetGraphicsRootSignature(rootSignature_->GetRootSignature(RootSignature::SPRITE));
    commandList->SetPipelineState(PSO::GetGraphicsPipelineStateSprite(blendMode).Get());//PSOを設定
    //形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておけばよい。
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void SpriteCommon::DrawCall(ID3D12GraphicsCommandList* commandList)
{
    //描画!（DrawCall/ドローコール）6個のインデックスを使用し1つのインスタンスを描画。その他は当面0で良い。
    commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void SpriteCommon::CreateIndexResource() {

#pragma region//IndexResourceを作成

    indexResource_.CreateBufferResource(L"SpriteCommon_IndexResource", sizeof(uint32_t) * 6);

    //Viewを作成する IndexBufferView(IBV)

    //リソースの先頭アドレスから使う
    indexBufferView_.BufferLocation = indexResource_.GetGPUVirtualAddress();
    //使用するリソースのサイズはインデックス6つ分のサイズ
    indexBufferView_.SizeInBytes = sizeof(uint32_t) * 6;
    //インデックスはuint32_tとする
    indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
#pragma endregion

#pragma region//IndexResourceにデータを書き込む
    //インデックスリーソースにデータを書き込む
    indexResource_.Map();

    //頂点数を削減
    indexResource_.data[0] = 0;
    indexResource_.data[1] = 1;
    indexResource_.data[2] = 2;

    indexResource_.data[3] = 1;
    indexResource_.data[4] = 3;
    indexResource_.data[5] = 2;

    indexResource_.UnMap();


#pragma endregion
}

