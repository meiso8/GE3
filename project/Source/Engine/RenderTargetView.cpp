#include "RenderTargetView.h"
#include"GetCPUDescriptorHandle.h"
#include<cassert>
#pragma comment(lib,"d3d12.lib")

void RenderTargetView::Create(
    const Microsoft::WRL::ComPtr<ID3D12Device>& device,
    const Microsoft::WRL::ComPtr <ID3D12Resource>(&swapChainResources)[2],
    const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& rtvDescriptorHeap) {

    const uint32_t descriptorSize= device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    //Descriptorは必ずDescriptorHandleというポインタのようなものを介して扱う必要がある
    //Viewを作るときは、どこのDescriptorに情報を格納するかを明示的に指定する必要がる
    rtvDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;//出力結果をSRGBに変換して書き込む
    rtvDesc_.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;//2dテクスチャとして書き込む
    //ディスクリプタの先頭を取得する
    D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    // GetCPUDescriptorHandleForHeapStart();DescriptorHeapが管理する最初のDescriptorHandleを返す


    //まず1つ目を作る。1つ目は最初のところに作る。作る場所をこちらで指定してあげる必要がある
    rtvHandles_[0] = rtvStartHandle;
    device->CreateRenderTargetView(swapChainResources[0].Get(), &rtvDesc_, rtvHandles_[0]);
    //2つ目のディスクリプタハンドルを得る（自力で）

    rtvHandles_[1] = GetCPUDescriptorHandle(rtvDescriptorHeap.Get(), descriptorSize, 1);

    //GetDescriptorHandleIncrementSize() Descriptorのサイズは、最適化のため、GPUまたはドライバによって異なることが許可されている
    //なのでDirectX12に問い合わせて実際のサイズを取得する　このサイズはゲーム中に変化することはないので初期化時に取得しておけばよい

    //2つ目を作る
    device->CreateRenderTargetView(swapChainResources[1].Get(), &rtvDesc_, rtvHandles_[1]);

}

const D3D12_CPU_DESCRIPTOR_HANDLE& RenderTargetView::GetHandle(size_t index) const {
    assert(index < 2);
    return rtvHandles_[index];
}
