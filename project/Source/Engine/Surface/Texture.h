#pragma once

#include <fstream>
//ComPtr(コムポインタ)
#include<wrl.h>
#include<d3d12.h>

//Textureデータを読み込むためにDirectXTex.hをインクルード
#include"../externals/DirectXTex/DirectXTex.h"
//Textureの転送のために
#include"../externals/DirectXTex/d3dx12.h"

#include"CommandList.h"
#include"ShaderResourceView.h"

//テクスチャの読み込み関数
DirectX::ScratchImage LoadTexture(const std::string& filePath);

Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(
    const Microsoft::WRL::ComPtr<ID3D12Device>& device,
    const DirectX::TexMetadata& metadata);

//TextureResourceにデータを転送する　GPU
[[nodiscard]]
Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(
    const Microsoft::WRL::ComPtr<ID3D12Resource>& texture,
    const DirectX::ScratchImage& mipImages,
    const Microsoft::WRL::ComPtr<ID3D12Device>& device,
    const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList);

class Texture {
public:
    Texture(const Microsoft::WRL::ComPtr<ID3D12Device>& device, CommandList& commandList)
        :device_(device), commandList_(commandList), metadata_() {
    };
    void Load(const std::string& filePath);
    DirectX::TexMetadata& GetMetadata() { return metadata_; };
    Microsoft::WRL::ComPtr<ID3D12Resource>& GetTextureResource() { return textureResource_; };
private:
    const Microsoft::WRL::ComPtr<ID3D12Device>& device_;
    CommandList& commandList_;
    DirectX::TexMetadata metadata_ = {};
    Microsoft::WRL::ComPtr<ID3D12Resource> textureResource_;
    Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource_;
};
