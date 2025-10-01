#include "SwapChain.h"
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#include<cassert>

void SwapChain::Create(Window& window,
    const Microsoft::WRL::ComPtr<IDXGIFactory7>& dxgiFactory,
    const Microsoft::WRL::ComPtr<ID3D12CommandQueue>& commandQueue) {

    swapChainDesc_.Width = window.GetClientWidth();   //画面の幅。ウィンドウのクライアント領域を同じものにしておく
    swapChainDesc_.Height = window.GetClientHeight();//画面の高さ。ウィンドウのクライアント領域を同じものにしておく
    swapChainDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//色の形式
    swapChainDesc_.SampleDesc.Count = 1;//マルチサンプルしない
    swapChainDesc_.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;//描画のターゲットとして利用する
    swapChainDesc_.BufferCount = 2;//ダブルバッファ
    swapChainDesc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;//モニタにうつしたら、中身を破棄
    //コマンドキュー、ウィンドウハンドル、設定を渡して生成する
    HRESULT result = dxgiFactory->CreateSwapChainForHwnd(
        commandQueue.Get(),
        window.GetHwnd(), &swapChainDesc_,
        nullptr, nullptr,
        reinterpret_cast<IDXGISwapChain1**>(swapChain_.GetAddressOf()));
    assert(SUCCEEDED(result));

}

void SwapChain::GetBuffer(UINT buffer, Microsoft::WRL::ComPtr<ID3D12Resource>& resource) {
    HRESULT result = swapChain_->GetBuffer(buffer, IID_PPV_ARGS(resource.GetAddressOf()));
    // 上手く取得出来なければ起動できない  
    assert(SUCCEEDED(result));

    //必要なものが作れないような場合、実際の製品であれば、メッセージを表示するなどする必要がある
   //ただ学生のうちはassertで止めてしまえばいい　ここら辺が失敗するPCというのは対象にしていないし、企業も使っていない
}