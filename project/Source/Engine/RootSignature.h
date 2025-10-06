#pragma once

#include<d3d12.h>
#include<wrl.h>

class RootSignature
{
public:
    void Create();
   ID3D12RootSignature* GetRootSignature() { return rootSignature_.Get(); };
private:
    Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignature_ = { nullptr };
};

