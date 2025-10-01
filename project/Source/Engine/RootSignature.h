#pragma once

#include<d3d12.h>
#include<wrl.h>

class RootSignature
{
public:
    void Create(const Microsoft::WRL::ComPtr<ID3D12Device>& device);
    Microsoft::WRL::ComPtr <ID3D12RootSignature>& GetRootSignature(size_t index) { return rootSignature_[index]; };
private:
    Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignature_[2] = { nullptr };
};

