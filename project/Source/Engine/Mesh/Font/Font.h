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
#include"hlslTypeToCpp.h"

class Font
{
public:

    struct MaterialForFont {
        float4 color;
        float32_t4x4 uvTransform;
    };

    Font();
    ~Font();
    void Create(const TextureFactory::Handle& textureHandle, const Vector2& position, const Vector4& color = { 1.0f,1.0f,1.0f,1.0f },  const Vector2& size = {64.0f,64.0f},const Vector2& anchorPoint = {0.0f,0.0f});

    void Update();
    void UpdateAnchorPoint();
    static void PreDraw(uint32_t blendMode = BlendMode::kBlendModeNormal);
    void Draw();

    void SetSize(const Vector2& size) { size_ = size; };
    void SetPosition(const Vector2& position) { position_ = position; }
    void SetRotate(const float& rotate) { rotate_ = rotate; }
    void SetScale(const Vector2& scale) { scale_ = scale; };

    Vector2& GetSize() { return size_; }
    Vector2& GetScale() { return scale_; };
    float& GetRotate() { return rotate_; };
    Vector2& GetPosition() { return position_; };
    const Vector2& GetPosition() const { return position_; };

    void SetTexture(const TextureFactory::Handle& textureHandle);

    Vector4& GetColor() { return material_->color; };
    void SetColor(const Vector4& color) { material_->color = color; };

    void SetUVScale(const Vector3& scale) { uvTransform_.scale = scale; };
    void SetUVRotate(const Vector3& rotate) { uvTransform_.rotate = rotate; };
    void SetUVTranslate(const Vector3& translate) { uvTransform_.translate = translate; };
    Vector3& GetUVScale() { return uvTransform_.scale; };
    Vector3& GetUVRotate() { return uvTransform_.rotate; };
    Vector3& GetUVTranslate() { return uvTransform_.translate; };

    Vector2& GetAnchorPoint() { return anchorPoint_; }

    /// @brief アンカーポイント
    /// @param anchorPoint 0.0f~1.0f
    
    void SetAnchorPoint(const Vector2& anchorPoint) { 
        anchorPoint_ = anchorPoint;
        UpdateAnchorPoint();
    }
    void SetIsFlipX(const bool isFlipX) { isFlipX_ = isFlipX; };
    void SetIsFlipY(const bool isFlipY) { isFlipY_ = isFlipY; };
    bool& GetIsFlipX() { return isFlipX_; };
    bool& GetIsFlipY() { return isFlipY_; };

    void SetTextureLeftTop(const Vector2& leftTop) { textureLeftTop = leftTop; }
    void SetTextureSize(const Vector2& size) { textureSize = size; };
    Vector2& GetTextureLeftTop() { return textureLeftTop; };
    Vector2& GetTextureSize() { return textureSize; };
    void AdjustTextureSize(const Vector2& size);
    void SetInUse(bool inUse) { inUse_ = inUse; }
    bool IsInUse() const { return inUse_; }

private:
    void CreateVertex();
    void CreateUVTransformationMatrix();
    void CreateTransformationMatrix();
    void CreateMaterial(const Vector4& color);
    void UpdateUV();

private:
    bool inUse_ = false;

    uint32_t textureHandle_ = 0;
    Vector2 anchorPoint_ = { 0.0f,0.0f };
    bool isFlipX_ = false;
    bool isFlipY_ = false;
    Vector2 textureLeftTop = { 0.0f,0.0f };
    Vector2 textureSize = { 100.0f,100.0f };
    static ID3D12GraphicsCommandList* commandList;

    Microsoft::WRL::ComPtr <ID3D12Resource> vertexResource_{};
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
    VertexData* vertexData_ = nullptr;

    Microsoft::WRL::ComPtr <ID3D12Resource> transformationMatrixResource_ = nullptr;
    Vector2 position_ = { 0.0f,0.0f };
    float rotate_ = 0.0f;
    Vector2 size_ = { 0.0f,0.0f };
    Vector2 scale_ = { 1.0f,1.0f };
    EulerTransform transform_{};
    Matrix4x4 worldMatrix_{};
    TransformationMatrixFor2D* transformationMatrixData_ = nullptr;

    EulerTransform uvTransform_ = { 0.0f };
    Matrix4x4 uvTransformMatrix_{};

    Microsoft::WRL::ComPtr <ID3D12Resource> materialResource_ = nullptr;
    MaterialForFont* material_ = nullptr;

};

////Fontとposとの当たり判定
//bool IsCollision(const Vector2& pos, Font& font);
////Fontとposとの当たり判定
//bool (Font& font, const Vector2& pos);