#pragma once

#include"CommandQueue.h"
#include"commandList.h"
#include"DXGIFactory.h"
#include"GPU.h"
#include"SwapChain.h"
#include"DebugError.h"

#include"RenderTargetView.h"
#include"Fence.h"
#include"PSO/Depth.h"//StencilTextureの作成関数　奥行き
#include"PSO/CompileShader.h"

#include"Window.h"
#include<array>
#include"ImGuiClass.h"
#include"TransitionBarrier.h"

#include"RenderTexture/RenderTexture.h"

//Textureデータを読み込むためにDirectXTex.hをインクルード
#include"../externals/DirectXTex/DirectXTex.h"
//Textureの転送のために
#include"../externals/DirectXTex/d3dx12.h"

#include"Vector4.h"

class SrvDescriptorHeap;
class RtvDescriptorHeap;
class DsvDescriptorHeap;

class DirectXCommon
{
public:

private:

    struct DepthTextureData {
        Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource = nullptr;
        uint32_t srvIndex = 0;
        D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU = {};
        D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU = {};
    };
private:
    Window* window_ = nullptr;
    DXGIFactory dxgiFactory = {};

    static Microsoft::WRL::ComPtr<ID3D12Device> device;
    static std::unique_ptr< DxcCompiler> dxcCompiler;
    std::unique_ptr<CommandList> commandList_ = nullptr;

    CommandQueue commandQueue = {};
    SwapChain swapChainClass;
    GPU gpu = {};
    DebugError debugError = {};
    std::array<Microsoft::WRL::ComPtr <ID3D12Resource>, 2> swapChainResources;
    RenderTargetView rtvClass = {};
    RenderTexture* renderTexture_ = nullptr;
    Fence fence = {};

    DepthTextureData depthTextureData_;

    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
    D3D12_VIEWPORT viewport = {};
    D3D12_RECT scissorRect = {};
    TransitionBarrier barrier = {};
public:

    void Finalize();
    ~DirectXCommon();
    
    /// @brief 初期化
    /// @param window windowクラスを渡す
    void PreInitialize(Window& window);
    void PostInitialize();
   
    void CreateDepthStencilResourceSRV(SrvDescriptorHeap* srvDescriptorHeap);
    /// @brief 描画前処理
/// @param color 画面の色を指定する
    void RenderTexturePreDraw(DsvDescriptorHeap* dsvDescriptorHeap);
    void VeiwPortAndScissorRect();
    void DrawRenderTexture(RtvDescriptorHeap* rtvDescriptorHeap);
    void RenderTexturePostDraw();
    void SetRenderTextureCamera(Camera* camera);
    void SettingIdTextureBarrierPre();
    void SettingIdTextureBarrierPost();

    /// @brief 描画前処理
    void PreDraw();
    /// @brief 描画後処理
    void PostDraw();
    /// @brief 次フレームの準備
    void PrepareCommand();
    /// @brief スワップチェインの取得関数
    /// @return スワップチェイン
    SwapChain& GetSwapChain() { return swapChainClass; };
    /// @brief スワップチェインのRTVの取得関数
    /// @return スワップチェインのRTV
    RenderTargetView& GetSwapChainRtv() {
        return rtvClass ;
    }
    
    void InitializeRenderTexture(RtvDescriptorHeap* rtvDescriptorHeap, SrvDescriptorHeap* srvDescriptorHeap);
    void InitializeRenderTargetView(RtvDescriptorHeap* rtvDescriptorHeap);
    void InitializeDepthStencilView(DsvDescriptorHeap* dsvDescriptorHeap);
    void UpdateGameScreen(SrvDescriptorHeap* srvDescriptorHeap);
    void UpdateRenderTexture(SrvDescriptorHeap* srvDescriptorHeap);
    /// @brief BufferResourceの作成関数
    /// @param sizeInBytes 
    /// @return BufferResource
    static Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(
        size_t sizeInBytes);
    /// @brief ReadbackBufferResource作成関数
    /// @param sizeInBytes 
    /// @return 
    static Microsoft::WRL::ComPtr<ID3D12Resource> CreateReadbackBufferResource(size_t sizeInBytes);
    /// @brief DescriptorHeapの作成関数
    /// @param heapType ヒープタイプの指定
    /// @param numDescriptors 
    /// @param shaderVisible 
    /// @return  DescriptorHeap
    static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(
        D3D12_DESCRIPTOR_HEAP_TYPE heapType,
        UINT numDescriptors,
        bool shaderVisible);

    /// @brief テクスチャリソースの作成関数
    /// @param metadata metadataの指定
    /// @return テクスチャリソース
    static Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(
        const DirectX::TexMetadata& metadata);
    /// @brief レンダーテクスチャの作成
    /// @param device デバイス
    /// @param width 幅
    /// @param height 高さ
    /// @param format フォーマット
    /// @param clearColor クリアカラー
    /// @return レンダーテクスチャ
    static Microsoft::WRL::ComPtr<ID3D12Resource>CreateRenderTextureResource(
        uint32_t width, uint32_t height,
        DXGI_FORMAT format,
        const Vector4& clearColor);

    /// @brief StencilTextureの作成関数　奥行き
    /// @param device 
    /// @param width 
    /// @param height 
    /// @return StencilTexture
   static Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencileTextureResource(
        int32_t width,
        int32_t height);

    /// @brief テクスチャデータの転送関数
    /// @param texture 
    /// @param mipImages 
    /// @return テクスチャデータ中間リソース
    [[nodiscard]]
    static Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(
        ID3D12GraphicsCommandList* commandList,
        const Microsoft::WRL::ComPtr<ID3D12Resource>& texture,
        const DirectX::ScratchImage& mipImages);
    /// @brief デバイスの取得関数
    /// @return  デバイス
    static Microsoft::WRL::ComPtr<ID3D12Device>& GetDevice() { return device; };
    /// @brief コンパイラーの取得関数
    /// @return コンパイラー
    static DxcCompiler* GetDxcCompiler() { return dxcCompiler.get(); }
    /// @brief コマンドリストの取得関数
    /// @return コマンドリスト
    ID3D12GraphicsCommandList* GetCommandList() { return commandList_->GetCommandList().Get(); };
    /// @brief コマンドリストクラスの取得関数
/// @return コマンドリスト
    CommandList* GetCommandListClass() { return commandList_.get(); };
private:
    void InitializeDevice();
    void InitializeCommand();
    void CreateSwapChain();
    void CreateDepthBuffer();


    void InitializeFence();
    void InitializeViewPort();
    void ScissorRectSetting();
    void CreateDXCCompiler();
};

