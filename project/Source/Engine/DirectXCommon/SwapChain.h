#pragma once
#include<dxgi1_6.h>
#include<d3d12.h>
//ComPtr(コムポインタ)
#include<wrl.h>
#include"Window.h"

class SwapChain
{
public:
    void Create(Window& window,
        const Microsoft::WRL::ComPtr<IDXGIFactory7>& dxgiFactory,
        const Microsoft::WRL::ComPtr<ID3D12CommandQueue>& commandQueue);
    Microsoft::WRL::ComPtr <IDXGISwapChain4>& GetSwapChain() {
        return swapChain_;
    };
    DXGI_SWAP_CHAIN_DESC1& GetDesc() {
        return swapChainDesc_
            ;
    };
    void GetBuffer(UINT buffer, Microsoft::WRL::ComPtr <ID3D12Resource>& resurce);

private:
    Microsoft::WRL::ComPtr <IDXGISwapChain4> swapChain_ = nullptr;
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc_{};
};

