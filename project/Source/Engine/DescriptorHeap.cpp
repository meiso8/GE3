#include"DescriptorHeap.h"
#include<cassert> //assertも利用するため

//DescriptorHeapの作成関数
Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(
    const Microsoft::WRL::ComPtr<ID3D12Device>& device,
    D3D12_DESCRIPTOR_HEAP_TYPE heapType,
    UINT numDescriptors,
    bool shaderVisible) {

    //ディスクリプタヒープを生成する
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap = nullptr;
    D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
    descriptorHeapDesc.Type = heapType;//連打ーターゲットビュー用
    descriptorHeapDesc.NumDescriptors = numDescriptors;
    descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
    //ディスクリプタヒープが作れなかったので起動できない
    assert(SUCCEEDED(hr));
    return descriptorHeap;
}