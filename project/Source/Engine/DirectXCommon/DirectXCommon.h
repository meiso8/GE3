#pragma once

#include"CommandQueue.h"
#include"commandList.h"
#include"DXGIFactory.h"
#include"GPU.h"
#include"SwapChain.h"
#include"DebugError.h"

#include"RenderTargetView.h"
#include"Fence.h"
#include"Depth.h"//StencilTextureの作成関数　奥行き
#include"CompileShader.h"

#include"Window.h"
#include<array>
#include"ImGuiClass.h"
#include"Vector4.h"
#include"TransitionBarrier.h"

//Textureデータを読み込むためにDirectXTex.hをインクルード
#include"../externals/DirectXTex/DirectXTex.h"
//Textureの転送のために
#include"../externals/DirectXTex/d3dx12.h"
#include<chrono>

class DirectXCommon
{
public:
    static uint32_t descriptorSizeRTV;
    static uint32_t descriptorSizeDSV;
    static uint32_t descriptorSizeSRV;
    static const uint32_t kMaxSRVCount;
private:
    Window* window_ = nullptr;
    DXGIFactory dxgiFactory = {};
    static Microsoft::WRL::ComPtr<ID3D12Device> device;
    static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap;
    //ゲームに一つだけ
    static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap;
    static Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> dsvDescriptorHeap;

    static std::unique_ptr< DxcCompiler> dxcCompiler;
    CommandQueue commandQueue = {};
    static std::unique_ptr<CommandList> commandList;
    SwapChain swapChainClass;
    GPU gpu = {};
    DebugError debugError = {};
    std::array<Microsoft::WRL::ComPtr <ID3D12Resource>, 2> swapChainResources;
    RenderTargetView rtvClass = {};
    Fence fence = {};
    Microsoft::WRL::ComPtr <ID3D12Resource> depthStencilResource = nullptr;

    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
    D3D12_VIEWPORT viewport = {};
    D3D12_RECT scissorRect = {};
    TransitionBarrier barrier = {};
    std::chrono::steady_clock::time_point reference_;

#ifdef _DEBUG
    ImGuiClass imGuiClass = {};
#endif // _DEBUG
public:
    void Initialize(Window& window);
    void Update();
    void PreDraw(Vector4& color);
    void PostDraw();
    void EndFrame();

   static Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(
        size_t sizeInBytes);

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(
        D3D12_DESCRIPTOR_HEAP_TYPE heapType,
        UINT numDescriptors,
        bool shaderVisible);

    static Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(
        const DirectX::TexMetadata& metadata);
    [[nodiscard]]
    static Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(
        const Microsoft::WRL::ComPtr<ID3D12Resource>& texture,
        const DirectX::ScratchImage& mipImages);
    static DirectX::ScratchImage LoadTextureFile(const std::string& filePath);
    static ID3D12Device* GetDevice() { return device.Get(); };
    static DxcCompiler* GetDxcCompiler() { return dxcCompiler.get(); }
    static ID3D12GraphicsCommandList* GetCommandList() { return commandList->GetCommandList().Get(); };
    ID3D12DescriptorHeap* GetSrvDescriptorHeap() { return srvDescriptorHeap.Get(); }
    static D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCPUDescriptorHandle(uint32_t index);
    static D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGPUDescriptorHandle(uint32_t index);
    static D3D12_CPU_DESCRIPTOR_HANDLE GetRTVCPUDescriptorHandle(uint32_t index);
    static D3D12_GPU_DESCRIPTOR_HANDLE GetRTVGPUDescriptorHandle(uint32_t index);
    static D3D12_CPU_DESCRIPTOR_HANDLE GetDSVCPUDescriptorHandle(uint32_t index);
    static D3D12_GPU_DESCRIPTOR_HANDLE GetDSVGPUDescriptorHandle(uint32_t index);


private:
    static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index);
    static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index);
    void InitializeDevice();
    void InitializeCommand();
    void CreateSwapChain();
    void CreateDepthBuffer();
    void DescriptorHeapSettings();
    void InitializeRenderTargetView();
    void InitializeDepthStencilView();
    void InitializeFence();
    void InitializeViewPort();
    void ScissorRectSetting();
    void CreateDXCCompiler();
    void InitializeImGui();
    void InitializeFixFPS();
    void UpdateFixFPS();
};

