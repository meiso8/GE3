#pragma once  

#include"PSO.h"  
#include"ShaderResourceView.h"  
#include"Camera/Camera.h"  
#include"Transform.h"  
#include"TransformationMatrix.h"  
#include"MaterialResource.h"  
#include"Vector2.h"  
#include"RootSignature.h"  

#include"Balloon.h"
#include"Wave.h"
#include<d3d12.h>
#include"SpriteC.h"

class Sprite
{
public:
    void Initialize(const Vector2& size = { 360.0f,640.0f });
    void UpdateUV();

    void PreDraw(uint32_t blendMode = BlendMode::kBlendModeNormal);
    void Draw(
        ShaderResourceView& srv, Camera& camera, uint32_t lightType = MaterialResource::LIGHTTYPE::NONE
    );


    void SetSize(const Vector2& size) { size_ = size; };
 
    void SetColor(const Vector4& color);
    void SetPosition(const Vector2& position) { position_ = position; }
    void SetRotate(const float& rotate) { rotate_ = rotate; }
    void SetScale(const Vector3& scale) { transform_.scale = scale; };
   
    void ResetSize(const Vector2& size);
    Vector2& GetSize() { return size_; }
    Vector3& GetScaleRef() { return transform_.scale; };
    float& GetRotateRef() { return rotate_; };
    Vector2& GetPositionRef() { return position_; };


    Material* GetMaterial() { return materialResource_.GetMaterial(); };
    Vector3& GetUVScale() { return uvTransform_.scale; };
    Vector3& GetUVRotate() { return uvTransform_.rotate; };
    Vector3& GetUVTranslate() { return uvTransform_.translate; };
    const Vector4& GetColor() { return materialResource_.GetMaterial()->color; }
private:
    void     CreateVertex();
    void CreateUVTransformationMatrix();
    void CreateTransformationMatrix();
    void CreateMaterial();
    void CreateWaveData();
    void CreateBalloonData();
private:
    static SpriteC* spriteCommon;
    ID3D12GraphicsCommandList* commandList;

    Microsoft::WRL::ComPtr <ID3D12Resource> vertexResource_{};
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
    VertexData* vertexData_ = nullptr;
    Vector2 size_;

    Microsoft::WRL::ComPtr <ID3D12Resource> transformationMatrixResource_ = nullptr;
    Vector2 position_ = { 0.0f,0.0f };
    float rotate_ = 0.0f;
    Transform transform_{};
    Matrix4x4 worldMatrix_{};
    Matrix4x4 worldViewProjectionMatrix_{};
    TransformationMatrix* transformationMatrixData_ = nullptr;

    Transform uvTransform_ = { 0.0f };
    Matrix4x4 uvTransformMatrix_{};

    MaterialResource materialResource_{};

    Microsoft::WRL::ComPtr<ID3D12Resource> expansionResource_;
    Balloon* expansionData_ = nullptr;

    Microsoft::WRL::ComPtr<ID3D12Resource> waveResource_;
    Wave* waveData = nullptr;

};