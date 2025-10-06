#include"Texture.h"
#include"Log.h"
#include<cassert>
#include"DirectXCommon.h"

Texture::Texture() {

}

void Texture::Load(const std::string& filePath) {
    DirectX::ScratchImage mipImages = DirectXCommon::LoadTextureFile(filePath);
    metadata_ = mipImages.GetMetadata();
    textureResource_ = DirectXCommon::CreateTextureResource(metadata_);
    intermediateResource_ = DirectXCommon::UploadTextureData(textureResource_.Get(), mipImages);
}
