#include "Font.h"
#include"DirectXCommon.h"
#include"TransformationMatrix.h"
#include"MakeMatrix.h"
#include"PSO.h"
#include"SrvDescriptorHeap.h"
#include"SpriteCamera.h"  
#include"Log.h"

ID3D12GraphicsCommandList* Font::commandList_ = nullptr;
/// @brief　SRV管理の借り物
CbvSrvUavDescriptorHeap* Font::srvDescriptorHeap_ = nullptr;
void Font::Create(
    const TextureFactory::Handle& textureHandle,
    const Vector2& position,
    const Vector4& color,
    const Vector2& size,
    const Vector2& anchorPoint)
{

    position_ = position;
    textureHandle_ = Texture::GetSRVHandle(textureHandle);
    anchorPoint_ = anchorPoint;

    CreateUVTransformationMatrix();
    CreateMaterial(color);

    CreateVertex();
    CreateTransformationMatrix();

    AdjustTextureSize(size);
}

void Font::Update()
{
    UpdateAnchorPoint();

    UpdateUV();
}

void Font::UpdateAnchorPoint()
{
    float left = 0.0f - anchorPoint_.x;
    float right = 1.0f - anchorPoint_.x;
    float top = 0.0f - anchorPoint_.y;
    float bottom = 1.0f - anchorPoint_.y;

    if (isFlipX_) {
        left = -left;
        right = -right;
    }

    if (isFlipY_) {
        top = -top;
        bottom = -bottom;
    }

    vertexResource_.data[0].position = { left,bottom,0.0f,1.0f };//左下
    vertexResource_.data[1].position = { left,top,0.0f,1.0f };//左上
    vertexResource_.data[2].position = { right,bottom,0.0f,1.0f };//右下
    vertexResource_.data[3].position = { right,top,0.0f,1.0f };//右上

    float texelWidth = 1.0f / textureSize.x;
    float texelHeight = 1.0f / textureSize.y;

    float offset = 0.5f;
    float tex_left = (textureLeftTop.x + offset) * texelWidth;
    float tex_right = (textureLeftTop.x + textureSize.x) * texelWidth;
    float tex_top = (textureLeftTop.y + offset) * texelHeight;
    float tex_bottom = (textureLeftTop.y + textureSize.y) * texelHeight;

    vertexResource_.data[0].texcoord = { tex_left,tex_bottom };
    vertexResource_.data[1].texcoord = { tex_left,tex_top };
    vertexResource_.data[2].texcoord = { tex_right,tex_bottom };
    vertexResource_.data[3].texcoord = { tex_right,tex_top };

}

void Font::SetTexture(const TextureFactory::Handle& textureHandle)
{
    textureHandle_ = Texture::GetSRVHandle(textureHandle);
}

void Font::SetCommandListAndSrvDescriptorHeap(ID3D12GraphicsCommandList* commandList, CbvSrvUavDescriptorHeap* srvDescriptorHeap)
{
    commandList_ = commandList;
    assert(commandList_);
    LogFile::Log("Font SetCommandList\n");

    srvDescriptorHeap_ = srvDescriptorHeap;
    assert(srvDescriptorHeap_);
    LogFile::Log("Font Set　SrvDescriptorHeap\n");
}

void Font::PreDraw(uint32_t blendMode) {

    //フォント用のRootSignature
    commandList_->SetGraphicsRootSignature(PSO::GetRootSignature()->GetRootSignature(RootSignature::FONT));
    //Font用のPSOの設定
    commandList_->SetPipelineState(PSO::GetGraphicsPipelineStateFont(blendMode).Get());//PSOを設定
    //形状を設定。
    commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Font::Draw(
) {

    transform_.scale = { scale_.x * size_.x,scale_.y * size_.y,1.0f };
    transform_.rotate = { 0.0f,0.0f,rotate_ };
    transform_.translate = { position_.x,position_.y,0.0f };

    worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);

    *transformationMatrixResource_.data = { Multiply(worldMatrix_, SpriteCamera::GetViewProjectionMatrix()),worldMatrix_ };

    //頂点バッファビューを設定
    commandList_->IASetVertexBuffers(0, 1, &vertexBufferView_);//VBVを設定
    //スプライト共通の処理
    SpriteCommon::SetIndexBuffer(commandList_);
    //マテリアルCBufferの場所を設定　/*RotParameter配列の0番目 0->register(b4)1->register(b0)2->register(b4)*/
    commandList_->SetGraphicsRootConstantBufferView(0, materialResource_.GetGPUVirtualAddress());
    //TransformationMatrixCBufferの場所を設定
    commandList_->SetGraphicsRootConstantBufferView(1, transformationMatrixResource_.GetGPUVirtualAddress());
    //SRVのDescriptorTableの先頭を設定。rootParameter[2]。
    srvDescriptorHeap_->SetGraphicsRootDescriptorTable(2, textureHandle_, commandList_);
    SpriteCommon::DrawCall(commandList_);

};

// ===================================//Private関数//===================================


void Font::CreateVertex()
{
    //VertexResourceとVertexBufferViewを用意 矩形を表現するための三角形を二つ(頂点4つ)
    vertexResource_.CreateBufferResource(L"Font_vertexResource", sizeof(VertexData) * 4);
    //頂点バッファビューを作成する
    //リソースの先頭アドレスから使う
    vertexBufferView_.BufferLocation = vertexResource_.resource->GetGPUVirtualAddress();
    //使用するリソースのサイズ頂点4つ分のサイズ
    vertexBufferView_.SizeInBytes = sizeof(VertexData) * 4;
    //1頂点あたりのサイズ
    vertexBufferView_.StrideInBytes = sizeof(VertexData);

#pragma region //Font用の頂点データの設定

    vertexResource_.Map();

    vertexResource_.data[0].position = { 0.0f,1.0f,0.0f,1.0f };//左下
    vertexResource_.data[1].position = { 0.0f,0.0f,0.0f,1.0f };//左上
    vertexResource_.data[2].position = { 1.0f,1.0f,0.0f,1.0f };//右下
    vertexResource_.data[3].position = { 1.0f,0.0f,0.0f,1.0f };//右上

    vertexResource_.data[0].texcoord = { 0.0f,1.0f };
    vertexResource_.data[0].normal = { 0.0f,0.0f,-1.0f };//法線

    vertexResource_.data[1].texcoord = { 0.0f,0.0f };
    vertexResource_.data[1].normal = { 0.0f,0.0f,-1.0f };

    vertexResource_.data[2].texcoord = { 1.0f,1.0f };
    vertexResource_.data[2].normal = { 0.0f,0.0f,-1.0f };

    vertexResource_.data[3].texcoord = { 1.0f,0.0f };
    vertexResource_.data[3].normal = { 0.0f,0.0f,-1.0f };

#pragma endregion

}

void Font::CreateUVTransformationMatrix()
{
    uvTransform_ = {
     {1.0f,1.0f,1.0f},
     {0.0f,0.0f,0.0f},
     {0.0f,0.0f,0.0f},
    };

    uvTransformMatrix_ = MakeIdentity4x4();
}

void Font::CreateTransformationMatrix() {

    //Matrix4x4　1つ分のサイズを用意
    transformationMatrixResource_.CreateBufferResource(L"Font_Transformation_Matrix_Resource");
    //書き込むためのアドレスを取得
    transformationMatrixResource_.Map();
    //データを書き込む
    transform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f} ,{ position_.x,position_.y,0.0f } };
    worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);

}

void Font::CreateMaterial(const Vector4& color) {

    //Matrix4x4　1つ分のサイズを用意
    materialResource_.CreateBufferResource(L"Font_materialResource");
    materialResource_.Map();

    materialResource_.data->color = color;
    materialResource_.data->uvTransform = MakeIdentity4x4();
    materialResource_.data->temperature = 0.5f;
}

void Font::UpdateUV() {
    uvTransformMatrix_ = MakeAffineMatrix(uvTransform_.scale, uvTransform_.rotate, uvTransform_.translate);
    materialResource_.data->uvTransform = uvTransformMatrix_;
}

void Font::AdjustTextureSize(const Vector2& size)
{
    textureSize = size;
    size_ = textureSize;
}

//bool IsCollision(const Vector2& pos, Font& font) {
//    Vector2 spritePos = font.GetPosition();
//    Vector2 spriteSize = font.GetSize(); // サイズが取得できる前提！
//
//    float left = spritePos.x - spriteSize.x * 0.5f;
//    float right = spritePos.x + spriteSize.x * 0.5f;
//    float top = spritePos.y - spriteSize.y * 0.5f;
//    float bottom = spritePos.y + spriteSize.y * 0.5f;
//
//    return (pos.x >= left && pos.x <= right &&
//        pos.y >= top && pos.y <= bottom);
//}
//
//bool IsCollision(Font& font, const Vector2& pos)
//{
//    return IsCollision(pos, font);
//}
