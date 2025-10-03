#pragma once

#include"CommandQueue.h"
#include"CommandList.h"
#include"DXGIFactory.h"
#include"GPU.h"
#include"SwapChain.h"
#include"DebugError.h"
#include"DescriptorHeap.h"
#include"RenderTargetView.h"
#include"Fence.h"
#include"FenceEvent.h"
#include"Depth.h"//StencilTextureの作成関数　奥行き
#include"CompileShader.h"
#include "RootSignature.h"


class DirctXBase
{
private:
    DXGIFactory dxgiFactory = {};
    Microsoft::WRL::ComPtr<ID3D12Device> device = nullptr;
#ifdef _DEBUG

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
    Microsoft::WRL::ComPtr <ID3D12Resource> depthStencilResource = nullptr;
    Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> dsvDescriptorHeap = nullptr;
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
    D3D12_VIEWPORT viewport = {};
    D3D12_RECT scissorRect = {};
public:
    void Initialize();
    void Update();
    void InitializeDevice();

    Microsoft::WRL::ComPtr<ID3D12Device>& GetDevice() { return device; };
    CommandList& GetCommandList() { return commandList; };
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& GetSrvDescriptorHeap() { return srvDescriptorHeap; }
};

