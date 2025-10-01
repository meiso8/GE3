#include"CreateBufferResource.h"

//Resource作成の関数化
Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(
    const Microsoft::WRL::ComPtr<ID3D12Device>& device,
    size_t sizeInBytes) {

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

    if (SUCCEEDED(device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
        &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
        IID_PPV_ARGS(&resource)))) {
        return resource;
    }

    return nullptr;

};
