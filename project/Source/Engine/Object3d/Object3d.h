#pragma once
#include<d3d12.h>
#include<wrl.h>
#include"TransformationMatrix.h"  
#include"Camera.h"  
#include"MeshCommon.h"
#include"WorldTransform.h"
#include"MaterialResource.h"  
#include"Balloon.h"
#include"Wave.h"
#include"Transform.h"
#include<memory>
class Model;
enum LightMode;
class Object3d
{

public:
    WorldTransform worldTransform_;
protected:

    //位置情報
    Microsoft::WRL::ComPtr <ID3D12Resource> transformationMatrixResource_ = nullptr;
    TransformationMatrixFor3D* transformationMatrixData_ = nullptr;


    //コマンドリスト 借り物
    static  ID3D12GraphicsCommandList* commandList_;

    //マテリアルリソース
    std::unique_ptr<MaterialResource> materialResource_ = nullptr;
    EulerTransform uvTransform_ = { 0.0f };
    Matrix4x4 uvTransformMatrix_{};


    //膨張データ
    Microsoft::WRL::ComPtr<ID3D12Resource> expansionResource_;
    Balloon* balloonData_ = nullptr;
    //波データ
    Microsoft::WRL::ComPtr<ID3D12Resource> waveResource_;
    Wave* waveData_ = nullptr;
private:
    //メッシュ情報
    Primitive* meshCommon_ = nullptr;

public:
    ~Object3d();

    Balloon& GetBalloonData() {
        return *balloonData_;
    }
    Wave& GetWaveData(size_t index) { return waveData_[index]; };

    void SetUV(const EulerTransform& transform) { uvTransform_ = transform; };
    void UpdateUV();

    Material& GetMaterial() { return *materialResource_->GetMaterial(); };

    Vector3& GetUVScale() { return uvTransform_.scale; };
    Vector3& GetUVRotate() { return uvTransform_.rotate; };
    Vector3& GetUVTranslate() { return uvTransform_.translate; };

    EulerTransform& GetUVTransform() {
        return uvTransform_;
    }

    int32_t& GetLightMode() {
        return materialResource_->GetMaterial()->lightMode;
    };

    Vector4& GetColor() { return materialResource_->GetMaterial()->color; };
    void SetColor(const Vector4& color) {
        materialResource_->SetColor(color);
    }

    void SetLightMode(const LightMode& lightMode) { materialResource_->SetLightMode(lightMode); }

    void Create();
    
    virtual void Initialize();
    virtual void Update();
    virtual void Draw(Camera& camera, const BlendMode& blendMode = BlendMode::kBlendModeNormal, const CullMode& cullMode = CullMode::kCullModeBack,const TextureFactory::Handle skyBoxTexture = TextureFactory::Handle::SKYBOX_TEX);
    void DrawForEffect(Camera& camera, const BlendMode& blendMode = BlendMode::kBlendModeAdd, const TextureFactory::Handle skyBoxTexture = TextureFactory::Handle::SKYBOX_TEX);

    void InitWaveData();
    void InitWaveDataIndex(const uint32_t& index);
    void InitBalloonData();

    void SetMesh(Primitive* mesh) { meshCommon_ = mesh; };

    virtual void SetTextureHandle(const TextureFactory::Handle& textureHandle) { meshCommon_->SetTextureHandle(textureHandle); };
private:
    void CreateUV();
    void CreateTransformationMatrix();
    void CreateMaterial(const Vector4& color = { 1.0f,1.0f,1.0f,1.0f }, const uint32_t& lightType = LightMode::kLightModeHalfL);
    void CreateWaveData();
    void CreateBalloonData();


};

