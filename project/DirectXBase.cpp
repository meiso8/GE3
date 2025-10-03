#include "DirectXBase.h"

using namespace Microsoft::WRL;

void DirectXBase::Initialize()
{

    //コマンドキューの生成
    commandQueue.Create(device);
    Log(logStream, "CreateCommandQueue");

    //コマンドリストの生成
    commandList.Create(device);
    Log(logStream, "CreateCommandList");

    //スワップチェインの生成
    //画面の幅。ウィンドウのクライアント領域を同じものにしておく
    swapChainClass.Create(
        wc,
        dxgiFactory.GetDigiFactory(),
        commandQueue.GetCommandQueue());
    Log(logStream, "CreateSwapChain");

#pragma region//DescriptorHeapを生成する
    rtvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);
    Log(logStream, "Create RTV DescriptorHeap");
#pragma endregion

#pragma region //SRV　SRVやCBV用のDescriptorHeapは一旦ゲーム中に一つだけ
    srvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, kMaxSRVCount, true);
    Log(logStream, "Create SRV DescriptorHeap");
#pragma endregion

    //TextureManagerの初期化
    //TextureManager::GetInstance()->Initialize();

#pragma region//SwapChainからResourceを引っ張ってくる
    //SwapChainからResourceを引っ張ってくる
    swapChainClass.GetBuffer(0, swapChainResources[0]);
    swapChainClass.GetBuffer(1, swapChainResources[1]);
    Log(logStream, "Pull Resource from SwapChain");

#pragma endregion

#pragma region//DescriptorSIze
    //DescriptorSizeを取得しておく
    //const uint32_t descriptorSizeDSV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
#pragma endregion

    //RTVを作る
    rtvClass.Create(device, swapChainResources, rtvDescriptorHeap);
    Log(logStream, "CreateRTV");

#pragma region //FenceとEventを生成する

    fence.Create(device);

    // FenceのSignalを持つためのイベントを作成する
    fenceEvent.Create();
    Log(logStream, "CreateFence&Event");
#pragma endregion

#pragma region// DXCの初期化　dxcCompilerを初期化
    dxcCompiler.Initialize();
    Log(logStream, "InitDxcCompiler");
#pragma endregion

#pragma region//RootSignatureを生成する

    //具体的にShaderがどこかでデータを読めばいいのかの情報を取りまとめたもの
    rootSignature.Create(device);
    Log(logStream, "CreateRootSignature");

#pragma endregion

#pragma region//stencileTextureResourceの作成 

    depthStencilResource = CreateDepthStencileTextureResource(device, wc.GetClientWidth(), wc.GetClientHeight());

    //DSV用ヒープでディスクリプタの数は1。DSVはShader内で触るものではないので、ShaderVisibleはfalse
    dsvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);

    //DSVの設定 DepthStencilView
    dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//基本的にはResourceに合わせる。
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;//2dTexture
    // DSVHeapの先頭にDSVを作る
    device->CreateDepthStencilView(depthStencilResource.Get(), &dsvDesc, dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

#pragma endregion

    //ViewportとScissor(シザー)
    viewport = CreateViewport(static_cast<float>(wc.GetClientWidth()), static_cast<float>(wc.GetClientHeight()));
    scissorRect = CreateScissorRect(wc.GetClientWidth(), wc.GetClientHeight());
    Log(logStream, "ViewportAndScissor");

    modelConfig_ = {
        &commandList,
        &rootSignature,
        directionalLightResource
    };

}

void DirectXBase::Update()
{


}

void DirctXBase::InitializeDevice()
{

    HRESULT hr;

#ifdef _DEBUG
    ComPtr <ID3D12Debug1> debugController = nullptr;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
        //デバッグレイヤーを有効化する
        debugController->EnableDebugLayer();
        //さらにGPU側でもチェックを行うようにする
        debugController->SetEnableGPUBasedValidation(TRUE);
    }
#endif

    //DXGIFactoryの生成
    dxgiFactory.Create();
    Log(logStream, "CreateDXGIFactory");

    GPU gpu = {};
    //使用するアダプタ(GPU)を決定する
    gpu.SettingGPU(dxgiFactory);
    Log(logStream, "Set GPU");

    //D3D12Deviceの生成
    device = CreateD3D12Device(gpu);
    Log("Complete create D3D12Device!!!\n");//初期化完了のログを出す
    //ファイルへのログ出力
    Log(logStream, "Complete create D3D12Device!!!\n");


    DebugError debugError = {};
#ifdef _DEBUG
    debugError.Create(device);
    Log(logStream, "SetDebugError");
#endif

}
