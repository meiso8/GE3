#pragma once
#include<d3d12.h>
#include<wrl.h>
#include"TransformationMatrix.h"  
#include"Camera.h"  
#include"Primitive.h"
#include"WorldTransform.h"
#include"MaterialResource.h"  
#include"Balloon.h"
#include"Wave.h"
#include"Transform.h"
#include<memory>

struct ObjectID {
    uint32_t id;
    uint32_t padding[3];
};

class Model;
enum LightMode;
class Object3d
{


protected:
    WorldTransform worldTransform_;
    // ==============位置情報==================
    Microsoft::WRL::ComPtr <ID3D12Resource> transformationMatrixResource_ = nullptr;
    TransformationMatrixFor3D* transformationMatrixData_ = nullptr;

    //マテリアルリソース
    std::unique_ptr<MaterialResource> materialResource_ = nullptr;
    EulerTransform uvTransform_ = { 0.0f };
    Matrix4x4 uvTransformMatrix_{};
    /// @brief テクスチャハンドル
    std::array<int32_t, TEXTURE_USAGE_COUNT> textureHandles_;

    // ==============膨張データ==================
    Microsoft::WRL::ComPtr<ID3D12Resource> expansionResource_;
    Balloon* balloonData_ = nullptr;

    // ==============波データ==================
    Microsoft::WRL::ComPtr<ID3D12Resource> waveResource_;
    Wave* waveData_ = nullptr;
    // ==============ID情報==================
    Microsoft::WRL::ComPtr <ID3D12Resource> idResource_ = nullptr;
    ObjectID* idData_ = nullptr;

    //メッシュ情報
    Primitive* primitive_ = nullptr;
private:

public:
    ~Object3d();
    // ==============位置情報==================
    WorldTransform& GetWorldTransform() { return worldTransform_; };
    EulerTransform& GetTransform() { return worldTransform_.eTransform_; }
    void SetTransform(const EulerTransform& transform) { worldTransform_.eTransform_ = transform; }
    const Matrix4x4& GetWorldMatrix() { return worldTransform_.matWorld_; };
    const Vector3& GetScale()const { return worldTransform_.eTransform_.scale; };
    const Vector3& GetRotate()const { return worldTransform_.eTransform_.rotate; };
    const Vector3& GetTranslate() const { return worldTransform_.eTransform_.translate; };
    void SetScale(const Vector3& scale) { worldTransform_.eTransform_.scale = scale; };
    void SetRotate(const Vector3& rotate) { worldTransform_.eTransform_.rotate = rotate; };
    void SetTranslate(const Vector3& translate) { worldTransform_.eTransform_.translate = translate; };

    // IDの設定と取得
    void SetObjectID(uint32_t id) { if (idData_) { idData_->id = id; } }
    uint32_t GetObjectID() const { if (idData_) { return idData_->id; }; return 0; }

    // ==============膨張データ==================

    Balloon& GetBalloonData() { return *balloonData_; }
    void InitBalloonData();

    // ==============波データ==================

    Wave& GetWaveData(size_t index) { return waveData_[index]; };
    void InitWaveData();
    void InitWaveDataIndex(const uint32_t& index);

    // ==============UVデータ==================

    Vector3& GetUVScale() { return uvTransform_.scale; };
    Vector3& GetUVRotate() { return uvTransform_.rotate; };
    Vector3& GetUVTranslate() { return uvTransform_.translate; };
    EulerTransform& GetUVTransform() { return uvTransform_; }
    void SetUV(const EulerTransform& transform) { uvTransform_ = transform; };
    void UpdateUV();

    // ==============マテリアルデータ==================
    Material& GetMaterial() { return *materialResource_->GetMaterial(); };

    int32_t& GetLightMode() { return materialResource_->GetMaterial()->lightMode; };
    void SetLightMode(const LightMode& lightMode) { materialResource_->SetLightMode(lightMode); }
    Vector4& GetColor() { return materialResource_->GetMaterial()->color; };
    void SetColor(const Vector4& color) { materialResource_->SetColor(color); }
    void SetTemperature(const float temp) { materialResource_->SetTemperature(temp); }
    uint32_t GetSrvIndex(const TEXTURE_USAGE& textureUsage) { return textureHandles_[textureUsage]; }
    virtual void SetTextureHandle(const TextureFactory::Handle& textureHandle, const TEXTURE_USAGE& textureUsage = TEXTURE_USAGE_DIFFUSE) {
        textureHandles_[textureUsage] = Texture::GetSRVHandle(textureHandle);
    };

    // ==============メッシュ==================
    Primitive* GetPrimitive() { return primitive_; }

    // ==============重要==================

 


    void SetMeshAndMaterial(Primitive* mesh);
    void Create();
    virtual void Initialize();
    void RegisterObject();
    virtual void Update();
    virtual void Draw(Camera& camera,
        const BlendMode& blendMode = kBlendModeNormal,
        const CullMode& cullMode = kCullModeBack,
        const MaskMode maskMode = kAll,
        const bool usePSOKey = false,
        const TextureFactory::Handle skyBoxTexture = TextureFactory::Handle::SKYBOX_TEX);
private:
    void CreateUV();
    void CreateTransformationMatrix();
    void CreateMaterial(const float temperature = 0.0f, const Vector4& color = { 1.0f,1.0f,1.0f,1.0f }, const uint32_t& lightType = LightMode::kLightModeHalfL);
    void CreateWaveData();
    void CreateBalloonData();
    void CreateID();

};

