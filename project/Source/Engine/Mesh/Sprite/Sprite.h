#pragma once  

#include"VertexData.h"
#include"PSO.h"  
#include"Transform.h"  
#include"TransformationMatrix.h"  

#include"Vector2.h"  
#include"RootSignature.h"  

#include<d3d12.h>
#include"SpriteCommon.h"

#include"Texture.h"
#include"../ResourceManager/ResourceManager.h"
class CommandList;

class Sprite
{
public:

    struct Material
    {
        float4 color;
        float32_t4x4 uvTransform;
        float temperature;
        float padding[3];
    };


public:
    static void SetCommandListAndSrvDescriptorHeap(
        ID3D12GraphicsCommandList* commandList,
        CbvSrvUavDescriptorHeap* srvDescriptorHeap
    );
public:
    void Create(const TextureFactory::Handle& textureHandle, const Vector2& position, const Vector4& color = { 1.0f,1.0f,1.0f,1.0f });
    ~Sprite();
    void Update();
    void UpdateAnchorPoint();
    static void PreDraw(uint32_t blendMode = BlendMode::kBlendModeNormal);
    void Draw();

    void SetColor(const Vector4& color);
    void SetTexture(const TextureFactory::Handle& textureHandle);
    void SetSize(const Vector2& size) { size_ = size; };
    void SetPosition(const Vector2& position) { position_ = position; }
    void SetRotate(const float& rotate) { rotate_ = rotate; }
    void SetScale(const Vector2& scale) { scale_ = scale; };

    void SetUVScale(const Vector3& scale) { uvTransform_.scale = scale; };
    void SetUVRotate(const Vector3& rotate) { uvTransform_.rotate = rotate; };
    void SetUVTranslate(const Vector3& translate) { uvTransform_.translate = translate; };

    Vector2& GetSize() { return size_; }
    Vector2& GetScale() { return scale_; };
    float& GetRotate() { return rotate_; };
    Vector2& GetPosition() { return position_; };
    const Vector2& GetPosition() const { return position_; };

    Material* GetMaterial() { return materialResource_.data; };
    Vector3& GetUVScale() { return uvTransform_.scale; };
    Vector3& GetUVRotate() { return uvTransform_.rotate; };
    Vector3& GetUVTranslate() { return uvTransform_.translate; };
    Vector4& GetColor() { return materialResource_.data->color; }
    const Vector4& GetColor() const { return materialResource_.data->color; }
    void SetTemperature(const float temperature)const { materialResource_.data->temperature = temperature;  }
    const float GetTemperature() { return materialResource_.data->temperature; }
    Vector2& GetAnchorPoint() { return anchorPoint_; }
    /// @brief アンカーポイント
    /// @param anchorPoint 0.0f~1.0f
    void SetAnchorPoint(const Vector2& anchorPoint) { anchorPoint_ = anchorPoint; UpdateAnchorPoint(); }
    void SetIsFlipX(const bool isFlipX) { isFlipX_ = isFlipX; };
    void SetIsFlipY(const bool isFlipY) { isFlipY_ = isFlipY; };
    bool& GetIsFlipX() { return isFlipX_; };
    bool& GetIsFlipY() { return isFlipY_; };

    void SetTextureLeftTop(const Vector2& leftTop) { textureLeftTop = leftTop; }
    void SetTextureSize(const Vector2& size) { textureSize = size; };
    Vector2& GetTextureLeftTop() { return textureLeftTop; };
    Vector2& GetTextureSize() { return textureSize; };
    void AdjustTextureSize();
private:
    void CreateVertex();
    void CreateUVTransformationMatrix();
    void CreateTransformationMatrix();
    void CreateMaterial(const Vector4& color);
    void UpdateUV();

private:
    //コマンドリストの借り物
    static ID3D12GraphicsCommandList* commandList_;
    /// @brief　SRV管理の借り物
    static  CbvSrvUavDescriptorHeap* srvDescriptorHeap_;
    //頂点リソース
    CResource<VertexData>vertexResource_;
    //座標リソース
    CResource<TransformationMatrixFor2D>transformationMatrixResource_;
    //マテリアルリソース
    CResource<Material>materialResource_;

    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

    uint32_t textureHandle_ = 0;
    Vector2 anchorPoint_ = { 0.0f,0.0f };
    bool isFlipX_ = false;
    bool isFlipY_ = false;
    Vector2 textureLeftTop = { 0.0f,0.0f };
    Vector2 textureSize = { 100.0f,100.0f };

    Vector2 position_ = { 0.0f,0.0f };
    float rotate_ = 0.0f;
    Vector2 size_ = { 0.0f,0.0f };
    Vector2 scale_ = { 1.0f,1.0f };
    EulerTransform transform_{};
    Matrix4x4 worldMatrix_{};

    EulerTransform uvTransform_ = { 0.0f };
    Matrix4x4 uvTransformMatrix_{};
};

//Spriteとposとの当たり判定
bool IsCollision(const Vector2& pos, Sprite& sprite);
//Spriteとposとの当たり判定
bool IsCollision(Sprite& sprite, const Vector2& pos);