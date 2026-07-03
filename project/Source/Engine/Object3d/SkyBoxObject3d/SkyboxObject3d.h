#pragma once
#include"Primitive.h"
#include"Camera.h"  
#include<memory>
#include"Object3d.h"

struct AABB;
class Model;
enum LightMode;

class SkyboxObject3d :public Object3d
{
public:
    struct MaterialForSkyBox
    {
        float4 color;
    };
private:
    //メッシュ情報
    std::unique_ptr<Primitive> skyBox_ = nullptr;
    //スカイボックスマテリアル
    MaterialForSkyBox* materialForSkyBox_ = nullptr;
public:
    ~SkyboxObject3d();
    /// @brief 色の取得　override
    /// @return 色
    Vector4& GetColor()override { return materialForSkyBox_->color; };
    /// @brief 色の設定　override
    void SetColor(const Vector4& color)override { materialForSkyBox_->color = color; };
    void Initialize()override;
    void Create();
    void Draw(Camera& camera);
private:
    void CreateMaterial(const Vector4& color = { 1.0f,1.0f,1.0f,1.0f });
};

