#pragma once
#include<d3d12.h>
#include"DirectXCommon.h"
#include"ModelConfig.h"
#include"VertexData.h"
class PSO;

class SpriteC
{
private:
    static SpriteC* instance_;
    DirectXCommon* directXCommon_;
    ModelConfig* modelConfig_ = nullptr;

    D3D12_INDEX_BUFFER_VIEW  indexBufferView_{};
    Microsoft::WRL::ComPtr <ID3D12Resource> indexResource_{};
    uint32_t* indexData_ = nullptr;
    Microsoft::WRL::ComPtr <ID3D12Resource> vertexResource_{};
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
    VertexData* vertexData_ = nullptr;
    Vector2 size_;
public:

    static SpriteC* GetInstance();
     void Initialize();
     void DrawVertex(ID3D12GraphicsCommandList* commandList);
     void LightDraw(ID3D12GraphicsCommandList* commandList);
     void PreDraw(ID3D12GraphicsCommandList* commandList);
     void DrawCall(ID3D12GraphicsCommandList* commandList);
    
     void SetSize(const Vector2& size);
     void ResetSize(const Vector2& size);
     Vector2* GetSize() { return &size_; }
private:
    void CreateVertex();
    void CreateIndexResource();
};

