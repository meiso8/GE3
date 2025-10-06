#pragma once

#include<wrl.h>
#include<d3d12.h>
#include<dxgi1_6.h>

#include"DXGIFactory.h"

class GPU
{
public:
    void SettingGPU(DXGIFactory& dxgiFactory);
    Microsoft::WRL::ComPtr <IDXGIAdapter4>& GetUseAdapter() {
        return useAdapter_;
    };
private:
    Microsoft::WRL::ComPtr <IDXGIAdapter4> useAdapter_ = nullptr;
};


Microsoft::WRL::ComPtr<ID3D12Device> CreateD3D12Device(GPU& gpu);