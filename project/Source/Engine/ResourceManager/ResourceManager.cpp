#include "ResourceManager.h"
#include"DirectXCommon.h"

#include "d3dx12.h"

void IResource::Reset()
{
    if (resource) {
        //あんまっぷしておく？
        resource.Reset();
    }
}


void IResource::UnMap()
{
    if (resource) {
        resource->Unmap(0, nullptr);
    }
}

void IResource::SetName(const LPCWSTR& label)
{
    if (resource) {
        resource->SetName(label);
    }
}

Microsoft::WRL::ComPtr<ID3D12Resource> ResourceFactory::CreateBufferResourceForUAV(size_t sizeInBytes) {

    Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;

    //ヒープの設定
    D3D12_HEAP_PROPERTIES heapProperties{};
    //GPUに近く置くためHeapをDefaultで作成する
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

    //頂点リソースの設定
    D3D12_RESOURCE_DESC resourceDesc{};

    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resourceDesc.Width = sizeInBytes;//リソースサイズ。
    //バッファの場合はこれらは1にする決まり
    resourceDesc.Height = 1;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.MipLevels = 1;
    resourceDesc.SampleDesc.Count = 1;
    //バッファの場合はこれにする決まり
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    //UAVの設定
    resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

    if (SUCCEEDED(DirectXCommon::GetDevice()->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        IID_PPV_ARGS(&resource)))) {
        return resource;
    }

    resource->SetName(L"DirectXCommon_UAV_BufferResource");

    return resource;

}
Microsoft::WRL::ComPtr<ID3D12Resource> ResourceFactory::CreateBufferResource(size_t sizeInBytes)
{
    Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;

    //ヒープの設定
    D3D12_HEAP_PROPERTIES uploadHeapProperties{};
    uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;//UploadHeapを使う
    //頂点リソースの設定
    D3D12_RESOURCE_DESC resourceDesc{};
    //バッファリソース。テクスチャの場合はまた別の設定をする
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resourceDesc.Width = sizeInBytes;//リソースサイズ。
    //バッファの場合はこれらは1にする決まり
    resourceDesc.Height = 1;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.MipLevels = 1;
    resourceDesc.SampleDesc.Count = 1;
    //バッファの場合はこれにする決まり
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    if (SUCCEEDED(DirectXCommon::GetDevice()->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
        &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
        IID_PPV_ARGS(&resource)))) {
        return resource;
    }

    resource->SetName(L"DirectXCommon_BufferResource");

    return resource;
}
Microsoft::WRL::ComPtr<ID3D12Resource> ResourceFactory::CreateReadbackBufferResource(size_t sizeInBytes)
{
    Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;

    // 1. ヒープの設定をREADBACKに変更
    D3D12_HEAP_PROPERTIES heapProperties{};
    heapProperties.Type = D3D12_HEAP_TYPE_READBACK; // ここが重要

    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resourceDesc.Width = sizeInBytes; // 1ピクセル分(UINTなら4バイト)
    resourceDesc.Height = 1;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.MipLevels = 1;
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    // 2. 初期状態を COPY_DEST にする
    if (SUCCEEDED(DirectXCommon::GetDevice()->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_COPY_DEST, // ここが重要
        nullptr,
        IID_PPV_ARGS(&resource)))) {
        return resource;
    }

    resource->SetName(L"DirectXCommon_ReadbackBufferResource");
    return resource;
}
Microsoft::WRL::ComPtr<ID3D12Resource> ResourceFactory::CreateTextureResource(const DirectX::TexMetadata& metadata)
{
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
    HRESULT hr = DirectXCommon::GetDevice()->CreateCommittedResource(
        &heapProperties,//Heapの設定
        D3D12_HEAP_FLAG_NONE,//Heapの特殊な設定。特になし
        &resourceDesc,//Resourceの設定
        D3D12_RESOURCE_STATE_COPY_DEST,// データ転送される設定
        nullptr,//Clear最適地。使わない
        IID_PPV_ARGS(&resource));//ポインタのポインタ

    resource->SetName(L"DirectXCommon_TextureResource");

    assert(SUCCEEDED(hr));

    return resource;
}

Microsoft::WRL::ComPtr<ID3D12Resource> ResourceFactory::CreateRenderTextureResource(uint32_t width, uint32_t height, DXGI_FORMAT format, const Vector4& clearColor)
{

    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Width = width;//Textureの値
    resourceDesc.Height = height;//Textureの高さ
    resourceDesc.MipLevels = 1;//mipmapの数
    resourceDesc.DepthOrArraySize = 1;//奥行き　or 配列Textureの配列数
    resourceDesc.Format = format;//TextureのFormat
    resourceDesc.SampleDesc.Count = 1;//サンプリングカウント。1固定。
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;//2次元
    resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;//RenderTarget

    //2.利用するHeapの設定
    D3D12_HEAP_PROPERTIES heapProperties{};
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;//GPUで処理するために書き換え

    D3D12_CLEAR_VALUE clearValue;
    clearValue.Format = format;
    clearValue.Color[0] = clearColor.x;
    clearValue.Color[1] = clearColor.y;
    clearValue.Color[2] = clearColor.z;
    clearValue.Color[3] = clearColor.w;

    //3.Resourceを生成する
    Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
    HRESULT hr = DirectXCommon::GetDevice()->CreateCommittedResource(
        &heapProperties,//Heapの設定
        D3D12_HEAP_FLAG_NONE,//Heapの特殊な設定。特になし
        &resourceDesc,//Resourceの設定
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,// SRVとする
        &clearValue,//Clear最適地。
        IID_PPV_ARGS(&resource));//ポインタのポインタ

    assert(SUCCEEDED(hr));

    return resource;
}
Microsoft::WRL::ComPtr<ID3D12Resource> ResourceFactory::CreateDepthStencileTextureResource(int32_t width, int32_t height)
{
    //生成するResourceの設定
    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Width = width;//Textureの幅
    resourceDesc.Height = height;//高さ
    resourceDesc.MipLevels = 1;//mipmapの数
    resourceDesc.DepthOrArraySize = 1;//奥行き　or 配列Textureの配列数
    resourceDesc.Format = DXGI_FORMAT_R32_TYPELESS;//DepthStencilとして利用可能なフォーマット
    resourceDesc.SampleDesc.Count = 1;//サンプリングカウント
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;//2次元
    resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;//DepthStencilとして使う通知

    //利用するHeapの設定
    D3D12_HEAP_PROPERTIES heapProperties{};
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;//VRAM上

    //深度値クリア設定
    D3D12_CLEAR_VALUE depthClearValue{};
    depthClearValue.DepthStencil.Depth = 1.0f;//1.0f(最大値)でクリア
    depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;//フォーマット。Resourceと合わせる

    //Resourceの生成
    Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
    HRESULT hr = DirectXCommon::GetDevice()->CreateCommittedResource(
        &heapProperties,//Heapの設定
        D3D12_HEAP_FLAG_NONE,//Heapの特殊な設定。特になし。
        &resourceDesc,//Resourceの設定
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,//深度値を最初は使う状態にする
        &depthClearValue,//Clear最適地
        IID_PPV_ARGS(&resource));
    assert(SUCCEEDED(hr));
    resource->SetName(L"DirectXCommon_DepthStencile_TextureResource");
    return resource;
}
Microsoft::WRL::ComPtr<ID3D12Resource> ResourceFactory::UploadTextureData(ID3D12GraphicsCommandList* commandList, const Microsoft::WRL::ComPtr<ID3D12Resource>& texture, const DirectX::ScratchImage& mipImages)
{
    std::vector<D3D12_SUBRESOURCE_DATA>subresources;
    DirectX::PrepareUpload(DirectXCommon::GetDevice().Get(), mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
    uint64_t intermediateSize = GetRequiredIntermediateSize(texture.Get(), 0, UINT(subresources.size()));
    Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = CreateBufferResource(intermediateSize);//中間リソース
    intermediateResource->SetName(L"texture:intermediateResource");

    UpdateSubresources(commandList, texture.Get(), intermediateResource.Get(), 0, 0, UINT(subresources.size()), subresources.data());
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
;


void IntermediateResource::UploadTextureData(const LPCWSTR& label, ID3D12GraphicsCommandList*commandList, ID3D12Resource* ohterResource, const DirectX::ScratchImage& mipImages)
{
    if (IResource::resource) {
        return;
    }

    IResource::resource = ResourceFactory::UploadTextureData(commandList, ohterResource,mipImages);
    IResource::resource->SetName(label);
}
