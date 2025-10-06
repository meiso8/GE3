#include "GPU.h"
#include"Log.h"

#include<cassert>
#include<format>
#include"StringUtility.h"

void GPU::SettingGPU(DXGIFactory& dxgiFactory) {

    //良い順にアダプタを頼む
    for (UINT i = 0;   dxgiFactory.GetDigiFactory()->EnumAdapterByGpuPreference(i,
        DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter_)) !=
        DXGI_ERROR_NOT_FOUND; ++i) {
        //アダプタの情報を取得する
        DXGI_ADAPTER_DESC3 adapterDesc{};
        HRESULT result = useAdapter_->GetDesc3(&adapterDesc);
        assert(SUCCEEDED(result));//取得できないのは一大事

        //ソフトウェアアダプタでなければ採用！
        if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
            //採用したアダプタ情報をログに出力。wstringの方なので注意
            LogFile::Log(StringUtility::ConvertString(std::format(L"Use Adapter:{}\n", adapterDesc.Description)));
            break;
        }

        useAdapter_ = nullptr;//ソフトウェアアダプタの場合は見なかったことにする

    }

    //適切なアダプタが見つからなかったので起動できない
    assert(useAdapter_ != nullptr);

}

Microsoft::WRL::ComPtr<ID3D12Device> CreateD3D12Device(GPU& gpu) {

    Microsoft::WRL::ComPtr<ID3D12Device> device = nullptr;

    //機能レベル(FEATURE_LEVEL)とログの出力用の文字列
    D3D_FEATURE_LEVEL featureLevels[] = {
      D3D_FEATURE_LEVEL_12_2,D3D_FEATURE_LEVEL_12_1,D3D_FEATURE_LEVEL_12_0
    };

    const char* featureLevelStrings[] = { "12.2","12.1","12.0" };
    //高い順に生成出来るか試していく
    for (size_t i = 0; i < _countof(featureLevels); ++i) {
        //採用したアダプターでデバイスを生成
        HRESULT hr = D3D12CreateDevice(gpu.GetUseAdapter().Get(), featureLevels[i], IID_PPV_ARGS(device.ReleaseAndGetAddressOf()));
        //指定した機能レベルでデバイスが生成できたかを確認する
        if (SUCCEEDED(hr)) {

            LogFile::Log(std::format("FeatureLevel : {}\n", featureLevelStrings[i]));
            break;
        }
    }

    //デバイスの生成が上手くいかなかったので起動できない
    assert(device != nullptr);

    return device;

}
