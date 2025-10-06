#include"Depth.h"
#include<cassert> //assertも利用するため

Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencileTextureResource(
    const Microsoft::WRL::ComPtr<ID3D12Device>& device,
    int32_t width,
    int32_t height) {
    //生成するResourceの設定
    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Width = width;//Textureの幅
    resourceDesc.Height = height;//高さ
    resourceDesc.MipLevels = 1;//mipmapの数
    resourceDesc.DepthOrArraySize = 1;//奥行き　or 配列Textureの配列数
    resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//DepthStencilとして利用可能なフォーマット
    resourceDesc.SampleDesc.Count = 1;//サンプリングカウント
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;//2次元
    resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;//DepthStencilとして使う通知

    //利用するHeapの設定
    D3D12_HEAP_PROPERTIES heapProperties{};
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;//VRAM上

    //深度値クリア設定
    D3D12_CLEAR_VALUE depthClearValue{};
    depthClearValue.DepthStencil.Depth = 1.0f;//1.0f(最大値)でクリア
    depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//フォーマット。Resourceと合わせる

    //Resourceの生成
    Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
    HRESULT hr = device->CreateCommittedResource(
        &heapProperties,//Heapの設定
        D3D12_HEAP_FLAG_NONE,//Heapの特殊な設定。特になし。
        &resourceDesc,//Resourceの設定
        D3D12_RESOURCE_STATE_DEPTH_WRITE,//深度値を書き込む状態にしておく
        &depthClearValue,//Clear最適地
        IID_PPV_ARGS(&resource));
    assert(SUCCEEDED(hr));

    return resource;
}



void DepthStencil::Create() {
    //Depthの機能を有効化する
    depthStencilDesc_.DepthEnable = true;
    //書き込みします
    depthStencilDesc_.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    //比較関数はLessEqual。つまり、近ければ描画される
    depthStencilDesc_.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
};
