#pragma once

#include<d3d12.h>
#include<wrl.h>
#include<cstdint>

class RootSignature
{
public:
   void Create();
   ID3D12RootSignature* GetRootSignature(uint32_t index) { return rootSignatures_[index].Get(); };

private:
    Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignatures_[2] = {nullptr};
};

