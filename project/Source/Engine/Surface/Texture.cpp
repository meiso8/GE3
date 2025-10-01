#include"Texture.h"
#include"Log.h"
#include"CreateBufferResource.h"

#include<cassert>

//テクスチャの読み込み関数
DirectX::ScratchImage LoadTexture(const std::string& filePath) {

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

Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(
    const Microsoft::WRL::ComPtr<ID3D12Device>& device,
    const DirectX::TexMetadata& metadata) {

    //1. metadataを基にResourceの設定
    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Width = UINT(metadata.width);//Textureの値
    resourceDesc.Height = UINT(metadata.height);//Textureの高さ
    resourceDesc.MipLevels = UINT16(metadata.mipLevels);//mipmapの数
    resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);//奥行き　or 配列Textureの配列数
    resourceDesc.Format = metadata.format;//TextureのFormat
    resourceDesc.SampleDesc.Count = 1;//サンプリングカウント。1固定。
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);//Textureの次元数。

    //2.利用するHeapの設定
    D3D12_HEAP_PROPERTIES heapProperties{};
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;//GPUで処理するために書き換え

    //3.Resourceを生成する
    Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
    HRESULT hr = device->CreateCommittedResource(
        &heapProperties,//Heapの設定
        D3D12_HEAP_FLAG_NONE,//Heapの特殊な設定。特になし
        &resourceDesc,//Resourceの設定
        D3D12_RESOURCE_STATE_COPY_DEST,// データ転送される設定
        nullptr,//Clear最適地。使わない
        IID_PPV_ARGS(&resource));//ポインタのポインタ

    assert(SUCCEEDED(hr));

    return resource;

}

//TextureResourceにデータを転送する　GPU
[[nodiscard]]
Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(
    const Microsoft::WRL::ComPtr<ID3D12Resource>& texture,
    const DirectX::ScratchImage& mipImages,
    const Microsoft::WRL::ComPtr<ID3D12Device>& device,
    const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList) {
    std::vector<D3D12_SUBRESOURCE_DATA>subresources;
    DirectX::PrepareUpload(device.Get(), mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
    uint64_t intermediateSize = GetRequiredIntermediateSize(texture.Get(), 0, UINT(subresources.size()));
    Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = CreateBufferResource(device, intermediateSize);//中間リソース
    UpdateSubresources(commandList.Get(), texture.Get(), intermediateResource.Get(), 0, 0, UINT(subresources.size()), subresources.data());
    //Textureへの転送後は利用できるよう,D3D12_RESOURCE_STATE_COPY_DESTからRESOURCE_STATE_GENERIC_READへResourceStateを変更する
    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = texture.Get();
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;//これに変更
    commandList->ResourceBarrier(1, &barrier);
    return intermediateResource;

}

void Texture::Load(const std::string& filePath) {
    DirectX::ScratchImage mipImages = LoadTexture(filePath);
    metadata_ = mipImages.GetMetadata();
    textureResource_ = CreateTextureResource(device_, metadata_);
    intermediateResource_ = UploadTextureData(textureResource_.Get(), mipImages, device_, commandList_.GetComandList());
}
