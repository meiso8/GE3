#include "Texture.h"
#include"DirectXCommon.h"
#include<cassert>
#include"SrvDescriptorHeap.h"
#include"Log.h"

std::vector<uint32_t> Texture::srvIndexes_;
std::unordered_map<uint32_t, std::filesystem::path> Texture::handleToPath_;
std::unordered_map<std::filesystem::path, Texture::TextureData> Texture::textureDatas;
ID3D12GraphicsCommandList* Texture::commandList_ = nullptr;
CbvSrvUavDescriptorHeap* Texture::srvDescriptorHeap_ = nullptr;

void Texture::Initialize()
{
    textureDatas.reserve(CbvSrvUavDescriptorHeap::kMaxCount_);
    // handles 配列を初期化（未ロード状態を示すために 0 で埋める）
    srvIndexes_.resize(TextureFactory::TEXTURES, 0);
}

void Texture::SetCommandListAndSrvDescriptorHeap(ID3D12GraphicsCommandList* commandList, CbvSrvUavDescriptorHeap* srvDescriptorHeap)
{
    commandList_ = commandList;
    assert(commandList_);
    srvDescriptorHeap_ = srvDescriptorHeap;
    assert(srvDescriptorHeap_);
}


void Texture::LoadAndMapHandle(const std::filesystem::path& filePath, const TextureFactory::Handle& handle)
{
    srvIndexes_[handle] = LoadAndGetIndex(filePath);
}

uint32_t Texture::LoadAndGetIndex(const std::filesystem::path& filePath)
{
    LoadTexture(filePath);

    uint32_t index = GetSrvIndexByFilePath(filePath);
    handleToPath_[index] = filePath;
    return index;
}

uint32_t Texture::AddTextureHandle(const std::filesystem::path& filePath) {

    uint32_t srvIndex = LoadAndGetIndex(filePath);
    // すでに登録済みならそのSRVインデックスを返す
    for (uint32_t existing : srvIndexes_) {
        if (existing == srvIndex) {
            return srvIndex;
        }
    }

    srvIndexes_.push_back(srvIndex);
    return srvIndex;
}

void Texture::AddTextureHandleByIndex(const uint32_t& srvIndex)
{
    // すでに登録済みならおしまい
    for (uint32_t existing : srvIndexes_) {
        if (existing == srvIndex) {
            return;
        }
    }

    srvIndexes_.push_back(srvIndex);

}

TextureFactory::Handle Texture::GetTextureHandle(const uint32_t& srvIndex)
{

    auto it = std::find(srvIndexes_.begin(), srvIndexes_.end(), srvIndex);
    if (it != srvIndexes_.end()) {
        // インデックスを取得して enum にキャスト 
        size_t index = std::distance(srvIndexes_.begin(), it);
        return static_cast<TextureFactory::Handle>(index);
    }
    assert(it != srvIndexes_.end());

    // 見つからなかった場合のフォールバック（適宜変更）
    return TextureFactory::WHITE_1X1;
}

void Texture::Finalize()
{
    for (auto& data : textureDatas) {
        data.second.resource.intermediateResource.Reset();
        data.second.resource.Reset();
    }

    textureDatas.clear();
}

uint32_t Texture::GetSrvIndexByFilePath(const std::filesystem::path& filePath)
{

    //読み込み済みテクスチャを検索
    if (textureDatas.contains(filePath)) {
        return textureDatas.at(filePath).resource.srvIndex;
    }

    assert(0);
    return 0;
}


D3D12_GPU_DESCRIPTOR_HANDLE Texture::GetSrvHandleGPU(const std::filesystem::path& filePath)
{
    //テクスチャ番号が正常範囲内にある
    assert(srvDescriptorHeap_->CanUseIndex());
    //テクスチャデータの参照を取得
    return  srvDescriptorHeap_->GetGPUDescriptorHandle(textureDatas[filePath].resource.srvIndex);
}

const DirectX::TexMetadata& Texture::GetMetaData(const uint32_t& handle)
{
    //テクスチャ番号が正常範囲内にある
    assert(srvDescriptorHeap_->CanUseIndex());

    return textureDatas[handleToPath_[handle]].resource.metadata;
}

void Texture::LoadTexture(const std::filesystem::path& filePath)
{  //読み込み済みテクスチャを検索

    if (textureDatas.contains(filePath)) {
        return;
    }

    //テクスチャ枚数上限チェック
    assert(srvDescriptorHeap_->CanUseIndex());

    //テクスチャファイルを読んでプログラムで扱えるようにする
    DirectX::ScratchImage image{};
    std::wstring filePathW = filePath.wstring();
    //sRBG空間で作られた物として読む。
    HRESULT hr;

    if (filePathW.ends_with(L".dds")) {
        //.ddsで終わっていたらddsとみなす。
        hr = DirectX::LoadFromDDSFile(filePathW.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);
    } else {
        hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
    }

    assert(SUCCEEDED(hr));
    const DirectX::TexMetadata metadata = image.GetMetadata();

    //ミニマップの作成
    DirectX::ScratchImage mipImages{};

    if (DirectX::IsCompressed(image.GetMetadata().format)) {
        //圧縮フォーマットである
        mipImages = std::move(image);
    } else {

        if (metadata.width > 1 || metadata.height > 1) {
            //hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
            hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 4, mipImages);
            assert(SUCCEEDED(hr));
        } else {
            mipImages = std::move(image); // そのまま使う
        }

    }

    //追加したテクスチャデータの参照を取得する
    TextureData& textureData = textureDatas[filePath];

    textureData.resource.metadata = mipImages.GetMetadata();

    textureData.resource.CreateTextureResource(L"TextureData_Resource");

    textureData.resource.UploadTextureData(L"intermediate_Resource", commandList_, mipImages);

    //テクスチャデータの要素数番号をSRVのインデックスとする
    textureData.resource.Allocate(srvDescriptorHeap_);

    //textureData.srvHandleCPU =  srvDescriptorHeap_->GetCPUDescriptorHandle(textureData.srvIndex);
    //textureData.srvHandleGPU =  srvDescriptorHeap_->GetGPUDescriptorHandle(textureData.srvIndex);
    textureData.resource.CreateSRVforTexture(srvDescriptorHeap_);
    //srvDescriptorHeap_->CreateSRVforTexture(textureData.srvIndex, textureData.resource.Get(), textureData.metadata);

}
