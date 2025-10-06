#pragma once
#include"commandList.h"
#include"PSO.h"
#include"RootSignature.h"

struct ModelConfig {
    RootSignature* rootSignature;
    ID3D12Resource* directionalLightResource;//共通のライトリソース
};
