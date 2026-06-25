#pragma once
#include<d3d12.h>
#include"DirectXCommon.h"

#include"PSO/RootSignature.h"
class CommandList;
class PSO;

class SpriteCommon
{
private:
    static RootSignature* rootSignature_;
    static D3D12_INDEX_BUFFER_VIEW  indexBufferView_;
    static Microsoft::WRL::ComPtr <ID3D12Resource> indexResource_;
    static uint32_t* indexData_;

public:
     void Finalize();
     void Initialize(RootSignature*  rootSignature);

     static void SetIndexBuffer(ID3D12GraphicsCommandList* commandList);
     static void PreDraw(uint32_t blendMode, ID3D12GraphicsCommandList* commandList);
     static void DrawCall(ID3D12GraphicsCommandList* commandList);
private:
    static void CreateIndexResource();
};

