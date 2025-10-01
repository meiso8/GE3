#include "CommandList.h"
#pragma comment(lib,"d3d12.lib")
#include<cassert> //assertも利用するため

//コマンドリストに必要な命令保存用メモリ管理構造を生成
void CommandList::Create(const Microsoft::WRL::ComPtr<ID3D12Device>& device) {

    HRESULT result = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_));
    //コマンドアロケータの生成が上手くいかなかったので起動できない
    assert(SUCCEEDED(result));

    //コマンドリストを生成する
    result = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr,
        IID_PPV_ARGS(&commandList_));
    //コマンドリスト生成が上手くいかなかったので起動できない
    assert(SUCCEEDED(result));
};

void CommandList::PrepareCommand() {

    HRESULT result = commandAllocator_->Reset();
    assert(SUCCEEDED(result));
    result = commandList_->Reset(commandAllocator_.Get(), nullptr);
    assert(SUCCEEDED(result));

};