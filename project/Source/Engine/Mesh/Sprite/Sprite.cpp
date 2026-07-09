#include "Sprite.h"
#include"DirectXCommon.h"
#include"TransformationMatrix.h"
#include"MakeMatrix.h"
#include"PSO.h"
#include"SrvDescriptorHeap.h"
#include"SpriteCamera.h"  
#include"ImGuiClass.h"
#include"CommandList.h"
#include"Log.h"

ID3D12GraphicsCommandList* Sprite::commandList_ = nullptr;
/// @brief　SRV管理の借り物
CbvSrvUavDescriptorHeap* Sprite::srvDescriptorHeap_ = nullptr;
void Sprite::SetCommandListAndSrvDescriptorHeap(ID3D12GraphicsCommandList* commandList, CbvSrvUavDescriptorHeap* srvDescriptorHeap)
{
    commandList_ = commandList;
    assert(commandList_);
    LogFile::Log("Sprite　SetCommandList");

    srvDescriptorHeap_ = srvDescriptorHeap;
    assert(srvDescriptorHeap_);
    LogFile::Log("Sprite Set　SrvDescriptorHeap\n");
}

void Sprite::Create(const TextureFactory::Handle& textureHandle, const Vector2& position, const Vector4& color)
{
    position_ = position;
    textureHandle_ = Texture::GetSRVHandle(textureHandle);

    CreateMaterial(color);
    CreateVertex();
    CreateTransformationMatrix();
    CreateUVTransformationMatrix();
    AdjustTextureSize();
}

Sprite::~Sprite()
{
    transformationMatrixResource_.Reset();
    materialResource_.Reset();
    vertexResource_.Reset();
}

void Sprite::Update()
{
    UpdateAnchorPoint();

    UpdateUV();
}

void Sprite::UpdateAnchorPoint()
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

    const DirectX::TexMetadata& metadata = Texture::GetMetaData(textureHandle_);

    float texelWidth = 1.0f / metadata.width;
    float texelHeight = 1.0f / metadata.height;
    float offset = 0.5f;
    float tex_left = (textureLeftTop.x + offset) * texelWidth;
    float tex_right = (textureLeftTop.x + textureSize.x - offset) * texelWidth;
    float tex_top = (textureLeftTop.y + offset) * texelHeight;
    float tex_bottom = (textureLeftTop.y + textureSize.y - offset) * texelHeight;


    vertexResource_.data[0].texcoord = { tex_left,tex_bottom };
    vertexResource_.data[1].texcoord = { tex_left,tex_top };
    vertexResource_.data[2].texcoord = { tex_right,tex_bottom };
    vertexResource_.data[3].texcoord = { tex_right,tex_top };

}

void Sprite::SetColor(const Vector4& color) {

    materialResource_.data->color = color;
}

void Sprite::SetTexture(const TextureFactory::Handle& textureHandle)
{
    textureHandle_ = Texture::GetSRVHandle(textureHandle);

}


void Sprite::PreDraw(uint32_t blendMode) {
    SpriteCommon::PreDraw(blendMode, commandList_);
}

void Sprite::Draw(
) {

    transform_.scale = { scale_.x * size_.x,scale_.y * size_.y,1.0f };
    transform_.rotate = { 0.0f,0.0f,rotate_ };
    transform_.translate = { position_.x,position_.y,0.0f };

    worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);

    *transformationMatrixResource_.data = { Multiply(worldMatrix_, SpriteCamera::GetViewProjectionMatrix()),worldMatrix_ };

    //頂点バッファビューを設定
    commandList_->IASetVertexBuffers(0, 1, &vertexBufferView_);//VBVを設定

    SpriteCommon::SetIndexBuffer(commandList_);

    //マテリアルCBufferの場所を設定　/*RotParameter配列の0番目 0->register(b4)1->register(b0)2->register(b4)*/
    commandList_->SetGraphicsRootConstantBufferView(0, materialResource_.GetGPUVirtualAddress());
    //TransformationMatrixCBufferの場所を設定
    commandList_->SetGraphicsRootConstantBufferView(1, transformationMatrixResource_.GetGPUVirtualAddress());
    //SRVのDescriptorTableの先頭を設定。rootParameter[2]
    srvDescriptorHeap_->SetGraphicsRootDescriptorTable(2, textureHandle_, commandList_);

    SpriteCommon::DrawCall(commandList_);

};

// ===================================//Private関数//===================================


void Sprite::CreateVertex()
{
    //VertexResourceとVertexBufferViewを用意 矩形を表現するための三角形を二つ(頂点4つ)
    vertexResource_.CreateBufferResource(L"Sprite_VertexResource", sizeof(VertexData) * 4);
    //頂点バッファビューを作成する
    //リソースの先頭アドレスから使う
    vertexBufferView_.BufferLocation = vertexResource_.GetGPUVirtualAddress();
    //使用するリソースのサイズ頂点4つ分のサイズ
    vertexBufferView_.SizeInBytes = sizeof(VertexData) * 4;
    //1頂点あたりのサイズ
    vertexBufferView_.StrideInBytes = sizeof(VertexData);

#pragma region //Sprite用の頂点データの設定

    vertexResource_.Map();
    //1枚目の三角形 四頂点でスプライト描画が完成

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

void Sprite::CreateUVTransformationMatrix()
{
    uvTransform_ = {
     {1.0f,1.0f,1.0f},
     {0.0f,0.0f,0.0f},
     {0.0f,0.0f,0.0f},
    };

    uvTransformMatrix_ = MakeIdentity4x4();
}

void Sprite::CreateTransformationMatrix() {

    //Matrix4x4　1つ分のサイズを用意
    transformationMatrixResource_.CreateBufferResource(L"Sprite_transformation_Matrix_Resource");

    //書き込むためのアドレスを取得
    transformationMatrixResource_.Map();

    //データを書き込む
    transform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f} ,{ position_.x,position_.y,0.0f } };
    worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);


}

void Sprite::CreateMaterial(const Vector4& color) {

    //Matrix4x4　1つ分のサイズを用意
    materialResource_.CreateBufferResource(L"Sprite_MaterialResource");
    materialResource_.Map();
  
    materialResource_.data->color = color;
    materialResource_.data->temperature = 0.5f;
    materialResource_.data->uvTransform = MakeIdentity4x4();
}

void Sprite::UpdateUV() {
    uvTransformMatrix_ = MakeAffineMatrix(uvTransform_.scale, uvTransform_.rotate, uvTransform_.translate);
    materialResource_.data->uvTransform = uvTransformMatrix_;
}

void Sprite::AdjustTextureSize()
{
    const DirectX::TexMetadata& metadata = Texture::GetMetaData(textureHandle_);
    textureSize.x = static_cast<float>(metadata.width);
    textureSize.y = static_cast<float>(metadata.height);
    size_ = textureSize;
}



bool IsCollision(const Vector2& pos, Sprite& sprite) {
    Vector2 spritePos = sprite.GetPosition();
    Vector2 spriteSize = sprite.GetSize(); // サイズが取得できる前提！

    float left = spritePos.x - spriteSize.x * 0.5f;
    float right = spritePos.x + spriteSize.x * 0.5f;
    float top = spritePos.y - spriteSize.y * 0.5f;
    float bottom = spritePos.y + spriteSize.y * 0.5f;

    return (pos.x >= left && pos.x <= right &&
        pos.y >= top && pos.y <= bottom);
}

bool IsCollision(Sprite& sprite, const Vector2& pos)
{
    return IsCollision(pos, sprite);
}
