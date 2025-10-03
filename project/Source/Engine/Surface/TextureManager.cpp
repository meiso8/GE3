#include "TextureManager.h"
#include"MyEngine.h"

#include<cassert>

uint32_t TextureManager::kSRVIndexTop = 1;

//テクスチャの読み込み関数
DirectX::ScratchImage LoadTextureFile(const std::string& filePath) {

    //テクスチャファイルを読んでプログラムで扱えるようにする
    DirectX::ScratchImage image{};
    std::wstring filePathW = ConvertString(filePath);
    //sRBG空間で作られた物として読む。
    HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
    assert(SUCCEEDED(hr));

    const DirectX::TexMetadata metadata = image.GetMetadata();

    //ミニマップの作成
    DirectX::ScratchImage mipImages{};

    if (metadata.width > 1 || metadata.height > 1) {
        hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);

        assert(SUCCEEDED(hr));
    } else {
        mipImages = std::move(image); // そのまま使う
    }

    //ミニマップ付きのデータを返す
    return mipImages;
}
TextureManager* TextureManager::instance = nullptr;

TextureManager* TextureManager::GetInstance()
{
    if (instance == nullptr) {
        instance = new TextureManager;
    }
    return instance;
}

void TextureManager::Finalize()
{
    delete instance;
    instance = nullptr;
}

void TextureManager::Initialize()
{
    //SRVの和と同数
    textureDatas.reserve(MyEngine::kMaxSRVCount);

}

void TextureManager::LoadTexture(const std::string& filePath)
{
    //読み込み済みテクスチャを検索
    auto it = std::find_if(
        textureDatas.begin(),
        textureDatas.end(),
        [&](TextureData& textureData) {return textureData.filePath == filePath; }
    );

    //テクスチャ枚数上限チェック
    assert(textureDatas.size() + kSRVIndexTop < MyEngine::kMaxSRVCount);

    if (it != textureDatas.end()) {
        return;
    }

    //テクスチャファイルを読んでプログラムで扱えるようにする
    DirectX::ScratchImage image{};
    std::wstring filePathW = ConvertString(filePath);
    //sRBG空間で作られた物として読む。
    HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
    assert(SUCCEEDED(hr));

    const DirectX::TexMetadata metadata = image.GetMetadata();

    //ミニマップの作成
    DirectX::ScratchImage mipImages{};

    if (metadata.width > 1 || metadata.height > 1) {
        hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);

        assert(SUCCEEDED(hr));
    } else {
        mipImages = std::move(image); // そのまま使う
    }

    //テクスチャデータを追加
    textureDatas.resize(textureDatas.size() + 1);
    //追加したテクスチャデータの参照を取得する
    TextureData& textureData = textureDatas.back();

    textureData.filePath = filePath;
    textureData.metadata = mipImages.GetMetadata();
    textureData.resource = CreateTextureResource(textureData.metadata);

    //テクスチャデータの要素数番号をSRVのインデックスとする
    uint32_t srvIndex = static_cast<uint32_t>(textureDatas.size() - 1) /*+ kSRVIndexTop*/;

    textureData.srvHandleCPU = GetCPUDescriptorHandle(srvIndex);
    textureData.srvHandleGPU = GetGPUDescriptorHandle(srvIndex);

    //metaDataを基にSRVの設定
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = textureData.metadata.format;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//texture
    srvDesc.Texture2D.MipLevels = UINT(textureData.metadata.mipLevels);

    //SRVの生成
    device->CreateShaderResourceView(textureData.resource.Get(), &srvDesc, textureData.srvHandleCPU);


}

uint32_t TextureManager::GetTextureIndexByFilePath(const std::string& filePath)
{
    //読み込み済みテクスチャデータを検索
    auto it = std::find_if(
        textureDatas.begin(),
        textureDatas.end(),
        [&](TextureData& textureData) {return textureData.filePath == filePath; }
    );

    if (it != textureDatas.end()) {
        uint32_t textureIndex = static_cast<uint32_t>(std::distance(textureDatas.begin(), it));
        return textureIndex;
    }

    assert(0);
    return 0;
}

D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetSrvHandleGPU(uint32_t textureIndex)
{

    //テクスチャ番号が正常範囲内にある
    assert(textureIndex + kSRVIndexTop < MyEngine::kMaxSRVCount);

    //テクスチャデータの参照を取得
    TextureData& textureData = textureDatas[textureIndex];

    return textureData.srvHandleGPU;
}
