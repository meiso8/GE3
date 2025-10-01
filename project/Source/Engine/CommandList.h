#pragma once
#include<d3d12.h>

//ComPtr(コムポインタ)
#include<wrl.h>

class CommandList
{
public:
    void Create(const Microsoft::WRL::ComPtr<ID3D12Device>& device);
    Microsoft::WRL::ComPtr <ID3D12GraphicsCommandList>& GetComandList() { return commandList_; };

    void PrepareCommand();

private:
    Microsoft::WRL::ComPtr <ID3D12GraphicsCommandList> commandList_ = nullptr;
    //コマンドアロケータを生成する(Allocate 割り当てる)
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_ = nullptr;
};

