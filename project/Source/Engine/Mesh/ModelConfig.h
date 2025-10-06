#pragma once
#include"CommandList.h"
#include"PSO.h"
#include"RootSignature.h"

struct ModelConfig {
    ID3D12GraphicsCommandList* commandList;
    RootSignature* rootSignature;
    ID3D12Resource* directionalLightResource;//共通のライトリソース
};
