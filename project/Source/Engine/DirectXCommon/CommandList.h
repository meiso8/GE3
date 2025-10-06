#pragma once
#include<d3d12.h>

//ComPtr(コムポインタ)
#include<wrl.h>

class CommandList
{
public:
    void Create();
    static Microsoft::WRL::ComPtr <ID3D12GraphicsCommandList>& GetCommandList() { return commandList_; };
    void PrepareCommand();

private:
    static Microsoft::WRL::ComPtr <ID3D12GraphicsCommandList> commandList_;
    //コマンドアロケータを生成する(Allocate 割り当てる)
   static Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_;
};

