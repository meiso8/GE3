#include "TransitionBarrier.h"
#include"Log.h"
#include<cassert>
void TransitionBarrier::SetCommandList(ID3D12GraphicsCommandList* commandList)
{
    commandList_ = commandList;
    assert(commandList_);
}

//TransitionBarrierの設定
void TransitionBarrier::SettingBarrier(
    const Microsoft::WRL::ComPtr <ID3D12Resource>& swapChainResources,
    D3D12_RESOURCE_STATES stateBefore,
    D3D12_RESOURCE_STATES stateAfter) {

    //今回はバリアはTransition
    barrier_.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    //Noneにしておく
    barrier_.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    //バリアを張る対象のリソース。現在のバックバッファに対して行う
    barrier_.Transition.pResource = swapChainResources.Get();
    //遷移前（現在）のResourceState
    barrier_.Transition.StateBefore = stateBefore;
    //遷移後のResourceState
    barrier_.Transition.StateAfter = stateAfter;

    barrier_.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    //TransitionBarrierを張る
    commandList_->ResourceBarrier(1, &barrier_);
}

void TransitionBarrier::SettingBarrierRTVforSRV( const Microsoft::WRL::ComPtr<ID3D12Resource>& swapChainResources)
{
    SettingBarrier(swapChainResources, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void TransitionBarrier::SettingBarrierSRVforRTV(const Microsoft::WRL::ComPtr<ID3D12Resource>& swapChainResources)
{
    SettingBarrier(swapChainResources, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
}


