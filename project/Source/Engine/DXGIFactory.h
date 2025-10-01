#pragma once
#include<d3d12.h>
#include<dxgi1_6.h>
//ComPtr(コムポインタ)
#include<wrl.h>

class DXGIFactory
{
public:
    void Create();
    Microsoft::WRL::ComPtr<IDXGIFactory7>& GetDigiFactory() {
        return dxgiFactory_;
    };

private:
    //DXGIファクトリー
    Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_ = nullptr;
};

