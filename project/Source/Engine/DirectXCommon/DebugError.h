#pragma once

//ComPtr(コムポインタ)
#include<wrl.h>
#include<d3d12.h>

class DebugError
{
public:
    void Create(const Microsoft::WRL::ComPtr<ID3D12Device>& device);

private:
    //デバイスに対してデバッグ
    Microsoft::WRL::ComPtr<ID3D12InfoQueue> infoQueue_ = nullptr;

};

