#pragma once
#include"MeshCommon.h"
#include<d3d12.h>
#include<wrl.h>
#include"TransformationMatrix.h"  
#include"Camera.h"  
#include"WorldTransform.h"

#include"Transform.h"
#include<memory>

struct MaterialForSkyBox
{
    float4 color;
};

struct AABB;
class Model;
enum LightMode;

class SkyboxObject3d
{
public:
    WorldTransform worldTransform_;
protected:

    //位置情報
    Microsoft::WRL::ComPtr <ID3D12Resource> transformationMatrixResource_ = nullptr;
    TransformationMatrixFor3DSkybox* transformationMatrixData_ = nullptr;
    //コマンドリスト 借り物
    static  ID3D12GraphicsCommandList* commandList_;
private:
    //メッシュ情報
    std::unique_ptr<Primitive> skyBox_ = nullptr;
    Microsoft::WRL::ComPtr <ID3D12Resource> materialResource_ = nullptr;
    MaterialForSkyBox* material_ = nullptr;
    int32_t textureHandle_ = -1;
public:
    ~SkyboxObject3d();

    Vector4& GetColor() { return material_->color; };
    void SetColor(const Vector4& color) {
        material_->color = color;
    }
    virtual void SetTextureHandle(const TextureFactory::Handle& textureHandle) {
        textureHandle_ = Texture::GetSRVHandle(textureHandle);
    };
    void Create();
    virtual void Initialize();
    virtual void Update();
    virtual void Draw(Camera& camera);
private:
    void CreateTransformationMatrix();
    void CreateMaterial(const Vector4& color = { 1.0f,1.0f,1.0f,1.0f });
};

