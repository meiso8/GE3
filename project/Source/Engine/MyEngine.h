#pragma once

#pragma region //自作関数
//#include"Window.h"
#include"Input.h"
#include"CommandQueue.h"
#include"CommandList.h"
#include"DXGIFactory.h"
#include"GPU.h"
#include"SwapChain.h"
#include"DescriptorHeap.h"
#include"RenderTargetView.h"
#include"GetCPUDescriptorHandle.h"
#include"GetGPUDescriptorHandle.h"
#include"DebugError.h"
#include"TransitionBarrier.h"
#include"Fence.h"
#include"FenceEvent.h"
#include"D3DResourceLeakChecker.h"
#include"Depth.h"//StencilTextureの作成関数　奥行き
#include"CompileShader.h"
#include"BlendState.h"
#include"RasterizerState.h"
#include"PSO.h"
#include"ViewPort.h"
#include"ScissorRect.h"
#include"Texture.h"
#include"CreateBufferResource.h"
#include"ShaderResourceView.h"
#include"Model.h"
#include"Sprite.h"
#include"SphereMesh.h"
#include"LineMesh.h"
#include"Sound.h"

#include"Camera/DebugCamera.h"
#include"Camera/Camera.h"
#include"CrashHandler.h"
#include"Log.h"
#include"ImGuiClass.h"
#include"InputLayout.h"
#include "RootSignature.h"

#include"FPSCounter.h"

#include"Material.h"
#include"VertexData.h"
#include"DirectionalLight.h"
#include"TransformationMatrix.h"
#include"ModelData.h"

#include"Normalize.h"
#include"Transform.h"
#include"MakeIdentity4x4.h"
#include"MakeAffineMatrix.h"
#include"Inverse.h"
#include"MakePerspectiveFovMatrix.h"
#include"MakeOrthographicMatrix.h"
#include"MakeRotateMatrix.h"
#include"Multiply.h"
#include"SphericalCoordinate.h"
#include"Lerp.h"

#include"DrawGrid.h"
#include"Cube.h"

#include"DebugUI.h"

#pragma endregion



class MyEngine {

public:
    static const uint32_t kMaxSRVCount;
    static uint32_t descriptorSizeSRV;
private:
    static MyEngine* instance_;

public:
 
    MyEngine() = default;
    ~MyEngine() = default;

    static MyEngine* GetInstance();

    void Create(const std::wstring& title, const int32_t clientWidth, const int32_t clientHeight);
    void Update();
    void PreCommandSet(Vector4& color);
    void PostCommandSet();
    void End();
    Window& GetWC() { return wc; };
    CommandList& GetCommandList() { return commandList; };
    ModelConfig& GetModelConfig() { return modelConfig_; };
    RootSignature& GetRootSignature() { return rootSignature; }
    PSO& GetPSO(uint32_t index) { return pso[index]; }
    Microsoft::WRL::ComPtr<ID3D12Device>& GetDevice() { return device; };
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& GetSrvDescriptorHeap() { return srvDescriptorHeap; }
    DirectionalLight& GetDirectionalLightData() { return *directionalLightData; }

    void SetBlendMode(uint32_t blendMode =BlendMode::kBlendModeNormal);

private:

    D3DResourceLeakChecker leakCheck = {};
    LogFile logFile = {};
    std::ofstream logStream;
    Window wc = {};
    DXGIFactory dxgiFactory = {};
    GPU gpu = {};
    Microsoft::WRL::ComPtr<ID3D12Device> device = nullptr;
    Input* input = { nullptr };
#ifdef _DEBUG
    DebugError debugError = {};
#endif
    CommandQueue commandQueue = {};
    CommandList commandList;
    SwapChain swapChainClass;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap = nullptr;
    //ゲームに一つだけ
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap = nullptr;
    Microsoft::WRL::ComPtr <ID3D12Resource> swapChainResources[2] = { nullptr };
    RenderTargetView rtvClass = {};
    Fence fence = {};
    FenceEvent fenceEvent = {};
    DxcCompiler dxcCompiler = {};
    RootSignature rootSignature = {};
    InputLayout inputLayout = {};
    std::vector<BlendState> blendStates = {};
    std::vector<RasterizerState> rasterizerStates = {};
    DepthStencil depthStencil = {};
    PSO pso[kCountOfBlendMode] = {};
    Microsoft::WRL::ComPtr <ID3D12Resource> depthStencilResource = nullptr;
    Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> dsvDescriptorHeap = nullptr;
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
    Microsoft::WRL::ComPtr <ID3D12Resource> directionalLightResource = nullptr;
    DirectionalLight* directionalLightData = nullptr;
    D3D12_VIEWPORT viewport = {};
    D3D12_RECT scissorRect = {};
#ifdef _DEBUG
    ImGuiClass imGuiClass = {};
#endif // _DEBUG

    TransitionBarrier barrier = {};
    ModelConfig modelConfig_ = {};
};

