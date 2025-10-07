#include "ModelConfig.h"

ModelConfig* ModelConfig::instance_ = nullptr;
RootSignature* ModelConfig::rootSignature = nullptr;
ID3D12Resource* ModelConfig::directionalLightResource = nullptr;//共通のライトリソース
ModelConfig* ModelConfig::GetInstance()
{
    if (instance_ == nullptr) {
        instance_ = new ModelConfig();
   }
    return instance_;
}

void ModelConfig::Initialize(RootSignature* rootSignature, ID3D12Resource* directionalLightResource)
{
    this->rootSignature =rootSignature;
    this->directionalLightResource =directionalLightResource;

}
