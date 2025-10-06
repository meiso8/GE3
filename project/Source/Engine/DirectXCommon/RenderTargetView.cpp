#include "RenderTargetView.h"
#include<cassert>
#pragma comment(lib,"d3d12.lib")
#include "DirectXCommon.h"

void RenderTargetView::Create(
   const std::array< Microsoft::WRL::ComPtr <ID3D12Resource>,2>swapChainResources) {

    //Descriptorは必ずDescriptorHandleというポインタのようなものを介して扱う必要がある
    //Viewを作るときは、どこのDescriptorに情報を格納するかを明示的に指定する必要がる
    rtvDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;//出力結果をSRGBに変換して書き込む
    rtvDesc_.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;//2dテクスチャとして書き込む

    //2つ作る。
    for (int i = 0; i < 2; ++i) {
        rtvHandles_[i] = DirectXCommon::GetRTVCPUDescriptorHandle(i);
        DirectXCommon::GetDevice()->CreateRenderTargetView(swapChainResources[i].Get(), &rtvDesc_, rtvHandles_[i]);
    }

}

const D3D12_CPU_DESCRIPTOR_HANDLE& RenderTargetView::GetHandle(size_t index) const {
    assert(index < 2);
    return rtvHandles_[index];
}
