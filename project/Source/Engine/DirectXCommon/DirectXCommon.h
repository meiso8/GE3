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
#include <memory>

#include"PostEffectMaterial/PostEffectMaterial.h"

//Textureデータを読み込むためにDirectXTex.hをインクルード
#include"../externals/DirectXTex/DirectXTex.h"

#include"Vector4.h"
#include"../RenderTexture/RenderTexture.h"

struct TextureResource;
class DepthTexture;
class CbvSrvUavDescriptorHeap;
class RtvDescriptorHeap;
class DsvDescriptorHeap;

class DirectXCommon
{
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
    std::unique_ptr<RenderTexture> renderTexture_ = nullptr;
    std::unique_ptr<RenderTexture> renderTextureForSprite_ = nullptr;

    Fence fence = {};

    D3D12_VIEWPORT viewport = {};
    D3D12_RECT scissorRect = {};
    TransitionBarrier barrier = {};
public:
    RenderTexture* GetRenderTexture() { return renderTexture_.get(); };
    void Finalize();
    ~DirectXCommon();
    
    /// @brief 初期化
    /// @param window windowクラスを渡す
    void PreInitialize(Window& window);
    void PostInitialize();

    /// @brief 描画前処理
/// @param color 画面の色を指定する
    void RenderTexturePreDraw(DsvDescriptorHeap* dsvDescriptorHeap, DepthTexture* depthTexture);
    void VeiwPortAndScissorRect();
    void DrawRenderTexture(RtvDescriptorHeap* rtvDescriptorHeap);
    void DrawRenderTextureForSprite(RtvDescriptorHeap* rtvDescriptorHeap);
    void RenderTexturePostDraw(DepthTexture* depthTexture);
 
    void SettingIdTextureBarrierPre();
    void SettingIdTextureBarrierPost();

    /// @brief 描画前処理
    void PreDraw();
    /// @brief 描画後処理
    void PostDraw();
    /// @brief 次フレームの準備
    void PrepareCommand();
    
    void RenderTextureForSpritePreDraw();
    void RenderTextureForSpritePostDraw();

    /// @brief スワップチェインの取得関数
    /// @return スワップチェイン
    SwapChain& GetSwapChain() { return swapChainClass; };
    /// @brief スワップチェインのRTVの取得関数
    /// @return スワップチェインのRTV
    RenderTargetView& GetSwapChainRtv() {
        return rtvClass ;
    }
    
    void InitializeRenderTexture(RtvDescriptorHeap* rtvDescriptorHeap, CbvSrvUavDescriptorHeap* srvDescriptorHeap);
    void InitializeRenderTargetView(RtvDescriptorHeap* rtvDescriptorHeap);

    void UpdateGameScreen(CbvSrvUavDescriptorHeap* srvDescriptorHeap);
    void UpdateRenderTexture(CbvSrvUavDescriptorHeap* srvDescriptorHeap);

    /// @brief DescriptorHeapの作成関数
    /// @param heapType ヒープタイプの指定
    /// @param numDescriptors 
    /// @param shaderVisible 
    /// @return  DescriptorHeap
    static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(
        D3D12_DESCRIPTOR_HEAP_TYPE heapType,
        UINT numDescriptors,
        bool shaderVisible);


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

    void InitializeFence();
    void InitializeViewPort();
    void ScissorRectSetting();
    void CreateDXCCompiler();
};

