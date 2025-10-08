#include "Fence.h"

#include<cassert>

void Fence::Create(const Microsoft::WRL::ComPtr <ID3D12Device>& device) {
    HRESULT hr;
    hr = device->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
    assert(SUCCEEDED(hr));
}

void Fence::SendSignal(CommandQueue& commandQueue)
{
    commandQueue.GetCommandQueue()->Signal(fence_.Get(), ++fenceValue_);
    //GetCompletedValueの初期値はFence作成時に渡した初期値
    if (fence_->GetCompletedValue() < fenceValue_) {

        HANDLE event = CreateEvent(NULL, FALSE, FALSE, NULL);
        assert(event != nullptr);
        //指定したSignalにたどり着いていないので、たどり着くまで待つようにイベントを設定する
        fence_->SetEventOnCompletion(fenceValue_, event);
        //イベントを待つ
        WaitForSingleObject(event, INFINITE);
        CloseHandle(event);
    }
}
