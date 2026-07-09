#pragma once
#include<wrl.h>
#include<d3d12.h>

class TransitionBarrier
{
public:
    void SetCommandList(ID3D12GraphicsCommandList* commandList);
    void SettingBarrier(
        const Microsoft::WRL::ComPtr <ID3D12Resource>& resource,
        D3D12_RESOURCE_STATES stateBefore,
        D3D12_RESOURCE_STATES stateAfter);
    void SettingBarrierRTVforSRV( const Microsoft::WRL::ComPtr <ID3D12Resource>& resource);
    void SettingBarrierSRVforRTV( const Microsoft::WRL::ComPtr <ID3D12Resource>& resource);
private:
    ID3D12GraphicsCommandList* commandList_ = nullptr;
    D3D12_RESOURCE_BARRIER barrier_{};
};

