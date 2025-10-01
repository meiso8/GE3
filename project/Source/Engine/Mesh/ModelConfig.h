#pragma once
#include"CommandList.h"
#include"PSO.h"
#include"RootSignature.h"

struct ModelConfig {
    CommandList* commandList;
    RootSignature* rootSignature;
    Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource;//共通のライトリソース
};
