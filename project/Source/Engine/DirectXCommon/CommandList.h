#pragma once
#include<d3d12.h>

//ComPtr(コムポインタ)
#include<wrl.h>

class CommandList
{
public:
    void Create();
    Microsoft::WRL::ComPtr <ID3D12GraphicsCommandList>& GetCommandList() { return commandList_; };
    ID3D12GraphicsCommandList* Get() { return commandList_.Get(); };
    void PrepareCommand();
private:
    Microsoft::WRL::ComPtr <ID3D12GraphicsCommandList> commandList_ = nullptr;
    //コマンドアロケータを生成する(Allocate 割り当てる)
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_ = nullptr;
};

