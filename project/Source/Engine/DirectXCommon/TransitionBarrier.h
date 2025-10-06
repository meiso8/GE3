#pragma once
#include<wrl.h>
#include<d3d12.h>

class TransitionBarrier
{
public:
    void SettingBarrier(
        const Microsoft::WRL::ComPtr <ID3D12Resource>& swapChainResources,
        ID3D12GraphicsCommandList* commandList);
    void Transition();
    const D3D12_RESOURCE_BARRIER& GetBarrier() { return barrier_; };
private:
    D3D12_RESOURCE_BARRIER barrier_{};
};

