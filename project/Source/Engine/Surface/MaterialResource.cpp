#include"MaterialResource.h"
#include"MakeMatrix.h"
#include"DirectXCommon.h"

MaterialResource::~MaterialResource()
{

}

void MaterialResource::UnMap()
{
    if (materialResource_) {
        materialResource_->Unmap(0, nullptr);
        materialResource_ = nullptr;
    }

    materialResource_.Reset();
}

void MaterialResource::CreateMaterial(const float temperature, const Vector4& color, uint32_t lightType) {

    //マテリアル用のリソースを作る。
    materialResource_ = DirectXCommon::CreateBufferResource(sizeof(Material));
    //マテリアルにデータを書き込む

    //書き込むためのアドレスを取得
    HRESULT result = materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&material_));
    material_->color = color;
    material_->lightMode = lightType;
    material_->uvTransform = MakeIdentity4x4();
    material_->shininess = 50.0f;
    material_->environmentCoefficient = 0.0f;
    //人の体温
    material_->temperature = temperature;
};


void MaterialResource::SetColor(const Vector4& color) {
    material_->color = color;
}



void MaterialResource::SetUV(const Matrix4x4& transform) {
    material_->uvTransform = transform;
}

void MaterialResource::SetLightMode(uint32_t lightType)
{
    material_->lightMode = lightType;
}

void MaterialResource::SetShininess(const float32_t& shininess)
{
    material_->shininess = shininess;
}

void MaterialResource::SetTemperature(const float32_t& temperature)
{
    material_->temperature = temperature;
}
