#pragma once

//ComPtr(コムポインタ)
#include<wrl.h>
#include<d3d12.h>
#include<cstdint>
#include"hlslTypeToCpp.h"

enum LightMode {
    kLightModeNone,
    kLightModeLReflectance,
    kLightModeHalfL,
};


struct Material
{
    float4 color;
    int32_t lightMode;
    float32_t shininess;
    float32_t environmentCoefficient;
    //温度を追加してみる
    float temperature;
    float32_t4x4 uvTransform;
};


class MaterialResource {

public:

    MaterialResource() = default;
    ~MaterialResource();
    void UnMap();

    void CreateMaterial(
        const float temperature = 0.0f,
        const Vector4& color = { 1.0f,1.0f,1.0f,1.0f }, 
        uint32_t lightType = LightMode::kLightModeNone, 
        const float shininess = 50.0f,
        const float  environmentCoefficient = 0.0f
        );
    Material* GetMaterial() {
        return material_;
    };
    const Material* GetMaterial() const {
        return material_;
    };

    Microsoft::WRL::ComPtr <ID3D12Resource>& GetMaterialResource() {
        return materialResource_
            ;
    };

    MaterialResource(const MaterialResource&) = delete;
    MaterialResource& operator=(const MaterialResource&) = delete;
    void SetColor(const Vector4& color);
    void SetUV(const Matrix4x4& transform);
    void SetLightMode(uint32_t lightType);
    void SetShininess(const float32_t& shininess);
    /// @brief 温度の設定
    /// @param temperature 温度
    void SetTemperature(const float32_t& temperature);
    void SetEnvironmentCoefficient(const float& environmentCoefficient) {
        material_->environmentCoefficient = environmentCoefficient;
    }
private:
    Microsoft::WRL::ComPtr <ID3D12Resource> materialResource_ = nullptr;
    Material* material_ = nullptr;
};