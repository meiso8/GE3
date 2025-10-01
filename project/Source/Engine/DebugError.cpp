#include "DebugError.h"
#include<cassert> //assertも利用するため

void DebugError::Create(const Microsoft::WRL::ComPtr<ID3D12Device>& device)
{
    if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue_)))) {
        //ヤバいエラー時に止まる
        infoQueue_->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
        //エラー時に止まる
        infoQueue_->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
        //警告時に止まる
        infoQueue_->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

        //解放を忘れたことが判明した場合、警告で停止する設定を外すことで、詳細な情報をログに出力することが出来る。
        //上記をコメントアウトし、情報を得て修正が終わったら必ず元に戻し停止しないことを確認する。


        //抑制するメッセージのID
        D3D12_MESSAGE_ID denyIds[] = {
            //Windows11でのDXGIデバッグレイヤーとDX12デバッグレイヤーの相互作用バグによるエラーメッセージ
                //https://stackoverflow.com/questions/69805245/directx-12-application-is-crashing-in-windows-11
                D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
        };

        //抑制するレベル
        D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
        D3D12_INFO_QUEUE_FILTER filter{};
        filter.DenyList.NumIDs = _countof(denyIds);
        filter.DenyList.pIDList = denyIds;
        filter.DenyList.NumSeverities = _countof(severities);
        filter.DenyList.pSeverityList = severities;
        //指定したメッセージの表示を抑制する
        infoQueue_->PushStorageFilter(&filter);

        //解放
        //infoQueue_->Release();
    }

}