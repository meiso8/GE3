#pragma once

#include<d3d12.h>
//ComPtr(コムポインタ)
#include<wrl.h>

//Textureデータを読み込むためにDirectXTex.hをインクルード
#include"../externals/DirectXTex/DirectXTex.h"

class Texture;

class ShaderResourceView
{
public:
    void Create(
        Texture& texture,
        uint32_t index,
        const Microsoft::WRL::ComPtr<ID3D12Device>& device,
        const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& srvDescriptorHeap
    );

    D3D12_GPU_DESCRIPTOR_HANDLE GetTextureSrvHandleGPU()const {
        return textureSrvHandleGPU_
            ;
    }
private:
    D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU_;
};

