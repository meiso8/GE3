#pragma once
#include"commandList.h"
#include"RootSignature.h"

class ModelConfig {
private:
    static ModelConfig* instance_;
public:
    static RootSignature* rootSignature;
    static ID3D12Resource* directionalLightResource;//共通のライトリソース
private:
public:
    static ModelConfig* GetInstance();
    void Initialize(RootSignature* rootSignature, ID3D12Resource* directionalLightResource);
};
