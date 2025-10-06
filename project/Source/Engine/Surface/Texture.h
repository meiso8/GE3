#pragma once

#include <fstream>
//ComPtr(コムポインタ)
#include<wrl.h>
#include<d3d12.h>

#include"CommandList.h"
#include"ShaderResourceView.h"

//TextureResourceにデータを転送する　GPU

class Texture {
public:
    Texture();
    void Load(const std::string& filePath);
    DirectX::TexMetadata& GetMetadata() { return metadata_; };
    Microsoft::WRL::ComPtr<ID3D12Resource>& GetTextureResource() { return textureResource_; };
private:
    DirectX::TexMetadata metadata_ = {};
    Microsoft::WRL::ComPtr<ID3D12Resource> textureResource_;
    Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource_;
};
