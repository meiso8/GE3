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
#include<chrono>

#include"Vector4.h"

class DirectXCommon
{
public:
    static uint32_t descriptorSizeRTV;
    static uint32_t descriptorSizeDSV;

    static const uint32_t kMaxSoundCount;
    static const uint32_t kMaxModelCount;
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
    static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap;

    static Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> dsvDescriptorHeap;
    static std::unique_ptr< DxcCompiler> dxcCompiler;
    static std::unique_ptr<CommandList> commandList;

    static float deltaTime_;

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
    std::chrono::steady_clock::time_point reference_;


private:
    // 1. コンストラクタとデストラクタを private にして、外部で new できないようにする
    DirectXCommon() = default;
    ~DirectXCommon();

public:

    // 2. コピーと代入を禁止する（インスタンスが2つに増えるのを防ぐため）
    DirectXCommon(const DirectXCommon&) = delete;
    DirectXCommon& operator=(const DirectXCommon&) = delete;
    // 3. インスタンスを取得するための GetInstance 関数（必ず static にする）
    static DirectXCommon* GetInstance()
    {
        // 関数内の static 変数は、プログラム実行中に1回だけ作られる
        // （C++11以降ではスレッドセーフが保証されているため安全です）
        static DirectXCommon instance;
        return &instance;
    }

    /// @brief 初期化
    /// @param window windowクラスを渡す
    void Initialize(Window& window);
    void CreateDepthStencilResourceSRV();
    /// @brief 描画前処理
/// @param color 画面の色を指定する
    void RenderTexturePreDraw();
    void DrawRenderTexture();
    void RenderTexturePostDraw();
    void SetRenderTextureCamera(Camera* camera);
    void SettingIdTextureBarrierPre();
    void SettingIdTextureBarrierPost();

    /// @brief 描画前処理
    void PreDraw();
    /// @brief 描画後処理
    void PostDraw();
    /// @brief フレーム終了処理
    void EndFrame();
    /// @brief スワップチェインの取得関数
    /// @return スワップチェイン
    SwapChain& GetSwapChain() { return swapChainClass; };
    /// @brief スワップチェインのRTVの取得関数
    /// @return スワップチェインのRTV
    RenderTargetView& GetSwapChainRtv() {
        return rtvClass
            ;
    }
    void InitializeRenderTexture();
    void UpdateRenderTexture();
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
        const Microsoft::WRL::ComPtr<ID3D12Resource>& texture,
        const DirectX::ScratchImage& mipImages);
    /// @brief デバイスの取得関数
    /// @return  デバイス
    static Microsoft::WRL::ComPtr<ID3D12Device> GetDevice() { return device; };
    /// @brief コンパイラーの取得関数
    /// @return コンパイラー
    static DxcCompiler* GetDxcCompiler() { return dxcCompiler.get(); }
    /// @brief コマンドリストの取得関数
    /// @return コマンドリスト
    static ID3D12GraphicsCommandList* GetCommandList() { return commandList->GetCommandList().Get(); };
    /// @brief RTVのCPUディスクリプタハンドルの取得関数
    /// @param index 
    /// @return RTVのCPUディスクリプタハンドル
    static D3D12_CPU_DESCRIPTOR_HANDLE GetRTVCPUDescriptorHandle(uint32_t index);
    /// @brief RTVのGPUディスクリプタハンドルの取得関数
    /// @param index 
    /// @return RTVのGPUディスクリプタハンドル
    static D3D12_GPU_DESCRIPTOR_HANDLE GetRTVGPUDescriptorHandle(uint32_t index);
    /// @brief DSVのCPUディスクリプタハンドルの取得関数
    /// @param index 
    /// @return DSVのCPUディスクリプタハンドル
    static D3D12_CPU_DESCRIPTOR_HANDLE GetDSVCPUDescriptorHandle(uint32_t index);
    /// @brief DSVのGPUディスクリプタハンドルの取得関数
    /// @param index 
    /// @return DSVのGPUディスクリプタハンドル
    static D3D12_GPU_DESCRIPTOR_HANDLE GetDSVGPUDescriptorHandle(uint32_t index);
    /// @brief 指定したディスクリプタヒープ内の指定インデックスに対応するCPUディスクリプタハンドルを取得。
    /// @param descriptorHeap 有効なID3D12DescriptorHeapへのポインタ。
    /// @param descriptorSize ディスクリプタ間のバイト単位のオフセット（ディスクリプタサイズ）。
    /// @param index ヒープ内の取得対象ディスクリプタのインデックス。
    /// @return 指定したインデックスに対応するD3D12_CPU_DESCRIPTOR_HANDLE。
    static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index);
    /// @brief 指定したディスクリプタヒープ内の指定インデックスに対応するGPUディスクリプタハンドルを取得。
    /// @param descriptorHeap 有効なID3D12DescriptorHeapへのポインタ。
    /// @param descriptorSize  ディスクリプタ間のバイト単位のオフセット（ディスクリプタサイズ）。
    /// @param index ヒープ内の取得対象ディスクリプタのインデックス。
    /// @return 指定したインデックスに対応するD3D12_GPU_DESCRIPTOR_HANDLE。
    static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index);
    static float GetDeltaTime() { return deltaTime_; };
private:
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
    void InitializeFixFPS();
    void UpdateFixFPS();

};

