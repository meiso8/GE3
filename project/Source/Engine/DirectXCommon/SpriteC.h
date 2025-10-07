#pragma once
#include<d3d12.h>
#include"DirectXCommon.h"
#include"ModelConfig.h"
class PSO;

class SpriteC
{
private:
    static SpriteC* instance_;
    DirectXCommon* directXCommon_;
    ModelConfig* modelConfig_{};

    D3D12_INDEX_BUFFER_VIEW  indexBufferView_{};
    Microsoft::WRL::ComPtr <ID3D12Resource> indexResource_{};
    uint32_t* indexData_ = nullptr;
public:
    static SpriteC* GetInstance();
    void Initialize(ModelConfig& mc);
    void Draw(ID3D12GraphicsCommandList* commandList);
    void PreDraw(ID3D12GraphicsCommandList* commandList);
private:

    void CreateIndexResource();
};

