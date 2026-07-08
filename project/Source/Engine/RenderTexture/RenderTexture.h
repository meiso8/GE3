#pragma once
#include<wrl.h>
#include<d3d12.h>
#include"Vector4.h"
#include<stdint.h>
#include<array>
class RtvDescriptorHeap;
class CbvSrvUavDescriptorHeap;
class CommandList;

class RenderTexture

{
public:
    
    enum RenderTextureType {
        kNormal0,
        kNormal1,
        kThermography,
        kObjectID,
        kMaxRenderTexutre,
    };
    struct RenderTextureData {
        Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
        uint32_t srvIndex = 0;
        D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU = {};
        D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU = {};
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandleCPU = {};
    };

private:

    Vector4 renderTargetClearValue_ = { 1.0f,1.0f,1.0f,1.0f };

    std::array< RenderTextureData, kMaxRenderTexutre> renderTextureDatas_;
    RenderTextureData thermographyTextureData_;

    //ID用リソース
    Microsoft::WRL::ComPtr<ID3D12Resource> idReadbackResource_ = nullptr;

    ID3D12GraphicsCommandList* commandList_ = nullptr;
    CbvSrvUavDescriptorHeap* srvDescriptorHeap_ = nullptr;
public:
    void Create(RtvDescriptorHeap* rtvDescriptorHeap, const Vector4& value);
  
    void SetCommandListAndSrvDescriptorHeap(ID3D12GraphicsCommandList* commandList, CbvSrvUavDescriptorHeap* srvDescriptorHeap);

    // 1. 描画コマンドの最後にコピー命令を積む関数 (PostDrawの直前に呼ぶ)
    void CopyClickPixelCommand(int mouseX, int mouseY);

    // 2. 次のフレームのUpdateなどで、安全にIDを読み出す関数
    uint32_t GetClickedObjectID();

    const Vector4& GetColor() {
        return renderTargetClearValue_;
    }

    RenderTextureData& GetRenderTextureData(const RenderTextureType index) {
        return renderTextureDatas_[index];
    }

    std::array< RenderTextureData, kMaxRenderTexutre>& GetRenderTextureDatas() { return renderTextureDatas_; };

    void Clear();

private:
    void CreateResource(
        const uint32_t index, 
        RtvDescriptorHeap* rtvDescriptorHeap,
        DXGI_FORMAT format,
        bool createSRV
    );

};

