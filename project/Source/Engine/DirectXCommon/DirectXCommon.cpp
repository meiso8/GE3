#include "DirectXCommon.h"
#include"Log.h"
#include "Viewport.h"
#include "ScissorRect.h"
#include<cassert>
#include"StringUtility.h"
#include <thread>

using namespace Microsoft::WRL;
ComPtr<ID3D12Device> DirectXCommon::device = nullptr;
ComPtr<ID3D12DescriptorHeap> DirectXCommon::srvDescriptorHeap = nullptr;
ComPtr<ID3D12DescriptorHeap> DirectXCommon::rtvDescriptorHeap = nullptr;
ComPtr<ID3D12DescriptorHeap> DirectXCommon::dsvDescriptorHeap = nullptr;

const uint32_t DirectXCommon::kMaxSRVCount = 512;
uint32_t DirectXCommon::descriptorSizeRTV = 0;
uint32_t DirectXCommon::descriptorSizeDSV = 0;
uint32_t DirectXCommon::descriptorSizeSRV = 0;

std::unique_ptr< DxcCompiler> DirectXCommon::dxcCompiler = nullptr;
std::unique_ptr<CommandList> DirectXCommon::commandList = nullptr;


void DirectXCommon::Initialize(Window& window)
{
    assert(&window);

    window_ = &window;

    InitializeFixFPS();
    InitializeDevice();
    InitializeCommand();
    CreateSwapChain();
    CreateDepthBuffer();
    DescriptorHeapSettings();
    InitializeRenderTargetView();
    InitializeDepthStencilView();
    InitializeFence();
    InitializeViewPort();
    ScissorRectSetting();
    CreateDXCCompiler();
    InitializeImGui();


}

void DirectXCommon::Update()
{
#ifdef _DEBUG

    //ImGuiにここからフレームが始まる旨を伝える
    imGuiClass.FrameStart();
#endif

}

void DirectXCommon::PreDraw(Vector4& color)
{

#ifdef _DEBUG
    //ImGuiの内部コマンドを生成する
    imGuiClass.Render();
#endif

    //これからの流れ
    //1.  BackBufferを決定する
    //2.  書き込む作業（画面のクリア）をしたいので、RTVを設定する
    //3.  画面のクリアを行う
    //4.  CommandListを閉じる
    //5.  CommandListの実行（キック）
    //6.  画面のスワップ（BackBufferとFrontBufferを入れ替える）
    //7.  次のフレーム用にCommandListを再準備

    //1.これから書き込むバックバッファのインデックスを取得
    UINT backBufferIndex = swapChainClass.GetSwapChain()->GetCurrentBackBufferIndex();

    //TransitionBarrierの設定

    barrier.SettingBarrier(swapChainResources[backBufferIndex], commandList->GetCommandList().Get());

    //2.描画用のRTVとDSVを設定する
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    commandList->GetCommandList()->OMSetRenderTargets(1, &rtvClass.GetHandle(backBufferIndex), false, &dsvHandle);
    //3.指定した色で画面全体をクリアする
    float clearColor[] = { color.x,color.y,color.z,color.w };//青っぽい色。RGBAの順
    commandList->GetCommandList()->ClearRenderTargetView(rtvClass.GetHandle(backBufferIndex), clearColor, 0, nullptr);

    //指定した深度で画面全体をクリアする
    commandList->GetCommandList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    //描画用のDescriptorHeapの設定
    ID3D12DescriptorHeap* descriptorHeaps[] = { srvDescriptorHeap.Get() };
    commandList->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps);

    //ビューポート領域の設定
    commandList->GetCommandList()->RSSetViewports(1, &viewport);//Viewportを設定
    //シザー矩形の設定
    commandList->GetCommandList()->RSSetScissorRects(1, &scissorRect);//Scirssorを設定



}

void DirectXCommon::PostDraw()
{
#ifdef _DEBUG
    //諸々の描画処理が終了下タイミングでImGuiの描画コマンドを積む
    imGuiClass.DrawImGui(*commandList);

#endif // _DEBUG


    //UINT backBufferIndex = swapChainClass.GetSwapChain()->GetCurrentBackBufferIndex();
    //barrier.SettingBarrier(swapChainResources[backBufferIndex], commandList->GetComandList());

    //画面に書く処理は終わり、画面に移すので、状態を遷移
    barrier.Transition();

    //TransitionBarrierを張る
    commandList->GetCommandList()->ResourceBarrier(1, &barrier.GetBarrier());

    //4.コマンドリストの内容を確定させる。全てのコマンドを詰んでから　Closesすること。
    HRESULT hr = commandList->GetCommandList()->Close();
    assert(SUCCEEDED(hr));
    LogFile::Log("CloseCommandList");

    //5.GPUにコマンドリストの実行を行わせる
    ID3D12CommandList* commandLists[] = { commandList->GetCommandList().Get() };
    commandQueue.GetCommandQueue()->ExecuteCommandLists(1, commandLists);
    //6.GPUとOSに画面の交換を行うよう通知する
    swapChainClass.GetSwapChain()->Present(1, 0);

    //画面の更新が終わった直後GPUにシグナルを送る
    //Fenceの値を更新
    //fence.AddValue();

    ////GPUがここまでたどり着いた時、Fenceの値を指定した値に代入するようにSignalを送る
    //fence.SendSignal(commandQueue);

    fence.Update(commandQueue);

    UpdateFixFPS();

    //Fenceの値が指定したSignal値にたどり着いているか確認する GPUの処理を待つ
    //fence.CheckValue(fenceEvent);

    //7.次のフレーム用のコマンドリストを準備
    commandList->PrepareCommand();

#pragma endregion

}

void DirectXCommon::EndFrame()
{

#ifdef _DEBUG
    //ImGuiの終了処理 ゲームループが終わったら行う
    imGuiClass.ShutDown();
#endif

    //CloseHandle(fenceEvent.GetEvent());

}


// =============================================================================================
void DirectXCommon::InitializeDevice()
{
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
    LogFile::Log("CreateDXGIFactory");


    //使用するアダプタ(GPU)を決定する
    gpu.SettingGPU(dxgiFactory);
    LogFile::Log("Set GPU");

    //D3D12Deviceの生成
    device = CreateD3D12Device(gpu);
    LogFile::Log("Complete create D3D12Device!!!\n");//初期化完了のログを出す
    //ファイルへのログ出力
    LogFile::Log("Complete create D3D12Device!!!\n");



#ifdef _DEBUG
    debugError.Create(device);
    LogFile::Log("SetDebugError");
#endif

}

void DirectXCommon::InitializeCommand()
{
    commandList = std::make_unique<CommandList>();
    //コマンドリストの生成
    commandList->Create();
    LogFile::Log("CreateCommandList");

    //コマンドキューの生成
    commandQueue.Create(device);
    LogFile::Log("CreateCommandQueue");

}

void DirectXCommon::CreateSwapChain()
{

    //スワップチェインの生成
//画面の幅。ウィンドウのクライアント領域を同じものにしておく
    swapChainClass.Create(
        *window_,
        dxgiFactory.GetDigiFactory(),
        commandQueue.GetCommandQueue());
    LogFile::Log("CreateSwapChain");

#pragma region//SwapChainからResourceを引っ張ってくる
    //SwapChainからResourceを引っ張ってくる
    swapChainClass.GetBuffer(0, swapChainResources[0]);
    swapChainClass.GetBuffer(1, swapChainResources[1]);
    LogFile::Log("Pull Resource from SwapChain");

#pragma endregion

}

void DirectXCommon::CreateDepthBuffer()
{
    //stencileTextureResourceの作成 

    depthStencilResource = CreateDepthStencileTextureResource(device, window_->GetClientWidth(), window_->GetClientHeight());

}

void DirectXCommon::DescriptorHeapSettings()
{
    //DescriptorSizeを取得しておく
    descriptorSizeRTV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    descriptorSizeSRV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    descriptorSizeDSV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

    //DescriptorHeapを生成する
    if (rtvDescriptorHeap == nullptr) {
        rtvDescriptorHeap = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);
        LogFile::Log("Create RTV DescriptorHeap");
    }

    //SRV　SRVやCBV用のDescriptorHeapは一旦ゲーム中に一つだけ
    if (srvDescriptorHeap == nullptr) {
        srvDescriptorHeap = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, kMaxSRVCount, true);
        LogFile::Log("Create SRV DescriptorHeap");

    }

    //DSV用ヒープでディスクリプタの数は1。DSVはShader内で触るものではないので、ShaderVisibleはfalse
    if (dsvDescriptorHeap == nullptr) {
        dsvDescriptorHeap = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);
        LogFile::Log("Create DSV DescriptorHeap");
    }


}

void DirectXCommon::InitializeRenderTargetView()
{
    //RTVを作る
    rtvClass.Create(swapChainResources);
    LogFile::Log("CreateRTV");

}

void DirectXCommon::InitializeDepthStencilView()
{
    //DSVの設定 DepthStencilView
    dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//基本的にはResourceに合わせる。
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;//2dTexture
    // DSVHeapの先頭にDSVを作る
    device->CreateDepthStencilView(depthStencilResource.Get(), &dsvDesc, dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

}

void DirectXCommon::InitializeFence()
{
#pragma region //FenceとEventを生成する
    fence.Create(device);
    LogFile::Log("CreateFence");
#pragma endregion
}

void DirectXCommon::InitializeViewPort()
{
    viewport = CreateViewport(static_cast<float>(window_->GetClientWidth()), static_cast<float>(window_->GetClientHeight()));
}

void DirectXCommon::ScissorRectSetting()
{
    //ViewportとScissor(シザー)
    scissorRect = CreateScissorRect(window_->GetClientWidth(), window_->GetClientHeight());
    LogFile::Log("ViewportAndScissor");


}

void DirectXCommon::CreateDXCCompiler()
{
    dxcCompiler = std::make_unique<DxcCompiler>();
    dxcCompiler->Initialize();
    dxcCompiler->ShaderSetting();
    LogFile::Log("InitDxcCompiler");

}

void DirectXCommon::InitializeImGui()
{
#ifdef _DEBUG
    //ImGuiの初期化。
    imGuiClass.Initialize(*window_, device.Get(), swapChainClass, rtvClass, srvDescriptorHeap);
    LogFile::Log("InitImGui");
#endif
}

void DirectXCommon::InitializeFixFPS()
{
    reference_ = std::chrono::steady_clock::now();

}

void DirectXCommon::UpdateFixFPS()
{
    //1/60秒ピッタリ
    const std::chrono::microseconds kMinTime(uint64_t(1000000.0f / 60.0f));
    //1/60秒よりわずかに短い時間
    const std::chrono::microseconds kMinCheckTime(uint64_t(1000000.0f / 65.0f));
    //現在時間を取得する
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    //前回記録からの経過時間を取得する
    std::chrono::microseconds elapsed =
        std::chrono::duration_cast<std::chrono::microseconds>(now - reference_);

    if (elapsed < kMinCheckTime) {
        //1/60秒経過するまで微小なスリープを繰り返す
        while (std::chrono::steady_clock::now() - reference_ < kMinTime) {
            //1μ秒スリープ
            std::this_thread::sleep_for(std::chrono::microseconds(1));
        }
    }

    //現在の時間を記録する
    reference_ = std::chrono::steady_clock::now();


}

// =============================================================================================

ComPtr<ID3D12Resource> DirectXCommon::CreateBufferResource(
    size_t sizeInBytes) {

    Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;

    //ヒープの設定
    D3D12_HEAP_PROPERTIES uploadHeapProperties{};
    uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;//UploadHeapを使う
    //頂点リソースの設定
    D3D12_RESOURCE_DESC resourceDesc{};
    //バッファリソース。テクスチャの場合はまた別の設定をする
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resourceDesc.Width = sizeInBytes;//リソースサイズ。
    //バッファの場合はこれらは1にする決まり
    resourceDesc.Height = 1;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.MipLevels = 1;
    resourceDesc.SampleDesc.Count = 1;
    //バッファの場合はこれにする決まり
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    if (SUCCEEDED(device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
        &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
        IID_PPV_ARGS(&resource)))) {
        return resource;
    }

    return nullptr;

};


ComPtr<ID3D12DescriptorHeap> DirectXCommon::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible)
{
    //ディスクリプタヒープを生成する
    ComPtr<ID3D12DescriptorHeap> descriptorHeap = nullptr;
    D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
    descriptorHeapDesc.Type = heapType;//連打ーターゲットビュー用
    descriptorHeapDesc.NumDescriptors = numDescriptors;
    descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
    //ディスクリプタヒープが作れなかったので起動できない
    assert(SUCCEEDED(hr));
    return descriptorHeap;
}

Microsoft::WRL::ComPtr<ID3D12Resource> DirectXCommon::CreateTextureResource(const DirectX::TexMetadata& metadata)
{
    //1. metadataを基にResourceの設定
    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Width = UINT(metadata.width);//Textureの値
    resourceDesc.Height = UINT(metadata.height);//Textureの高さ
    resourceDesc.MipLevels = UINT16(metadata.mipLevels);//mipmapの数
    resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);//奥行き　or 配列Textureの配列数
    resourceDesc.Format = metadata.format;//TextureのFormat
    resourceDesc.SampleDesc.Count = 1;//サンプリングカウント。1固定。
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);//Textureの次元数。

    //2.利用するHeapの設定
    D3D12_HEAP_PROPERTIES heapProperties{};
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;//GPUで処理するために書き換え

    //3.Resourceを生成する
    Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
    HRESULT hr = device->CreateCommittedResource(
        &heapProperties,//Heapの設定
        D3D12_HEAP_FLAG_NONE,//Heapの特殊な設定。特になし
        &resourceDesc,//Resourceの設定
        D3D12_RESOURCE_STATE_COPY_DEST,// データ転送される設定
        nullptr,//Clear最適地。使わない
        IID_PPV_ARGS(&resource));//ポインタのポインタ

    assert(SUCCEEDED(hr));

    return resource;
}

[[nodiscard]]
Microsoft::WRL::ComPtr<ID3D12Resource> DirectXCommon::UploadTextureData(const Microsoft::WRL::ComPtr<ID3D12Resource>& texture, const DirectX::ScratchImage& mipImages)
{
    std::vector<D3D12_SUBRESOURCE_DATA>subresources;
    DirectX::PrepareUpload(device.Get(), mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
    uint64_t intermediateSize = GetRequiredIntermediateSize(texture.Get(), 0, UINT(subresources.size()));
    Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = CreateBufferResource(intermediateSize);//中間リソース
    UpdateSubresources(commandList->GetCommandList().Get(), texture.Get(), intermediateResource.Get(), 0, 0, UINT(subresources.size()), subresources.data());
    //Textureへの転送後は利用できるよう,D3D12_RESOURCE_STATE_COPY_DESTからRESOURCE_STATE_GENERIC_READへResourceStateを変更する
    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = texture.Get();
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;//これに変更
    commandList->GetCommandList()->ResourceBarrier(1, &barrier);
    return intermediateResource;
}

DirectX::ScratchImage DirectXCommon::LoadTextureFile(const std::string& filePath)
{
    //テクスチャファイルを読んでプログラムで扱えるようにする
    DirectX::ScratchImage image{};
    std::wstring filePathW = StringUtility::ConvertString(filePath);
    //sRBG空間で作られた物として読む。
    HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
    assert(SUCCEEDED(hr));

    const DirectX::TexMetadata metadata = image.GetMetadata();

    //ミニマップの作成
    DirectX::ScratchImage mipImages{};

    if (metadata.width > 1 || metadata.height > 1) {
        hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);

        assert(SUCCEEDED(hr));
    } else {
        mipImages = std::move(image); // そのまま使う
    }

    //ミニマップ付きのデータを返す
    return mipImages;
}


D3D12_CPU_DESCRIPTOR_HANDLE DirectXCommon::GetSRVCPUDescriptorHandle(uint32_t index)
{
    return GetCPUDescriptorHandle(srvDescriptorHeap.Get(), descriptorSizeSRV, index);
}

D3D12_GPU_DESCRIPTOR_HANDLE DirectXCommon::GetSRVGPUDescriptorHandle(uint32_t index)
{
    return GetGPUDescriptorHandle(srvDescriptorHeap.Get(), descriptorSizeSRV, index);
}

D3D12_CPU_DESCRIPTOR_HANDLE DirectXCommon::GetRTVCPUDescriptorHandle(uint32_t index)
{
    return GetCPUDescriptorHandle(rtvDescriptorHeap.Get(), descriptorSizeRTV, index);
}

D3D12_GPU_DESCRIPTOR_HANDLE DirectXCommon::GetRTVGPUDescriptorHandle(uint32_t index)
{
    return  GetGPUDescriptorHandle(rtvDescriptorHeap.Get(), descriptorSizeRTV, index);
}

D3D12_CPU_DESCRIPTOR_HANDLE DirectXCommon::GetDSVCPUDescriptorHandle(uint32_t index)
{
    return GetCPUDescriptorHandle(dsvDescriptorHeap.Get(), descriptorSizeDSV, index);
}

D3D12_GPU_DESCRIPTOR_HANDLE DirectXCommon::GetDSVGPUDescriptorHandle(uint32_t index)
{
    return GetGPUDescriptorHandle(dsvDescriptorHeap.Get(), descriptorSizeDSV, index);
}

D3D12_CPU_DESCRIPTOR_HANDLE DirectXCommon::GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index)
{
    D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
    handleCPU.ptr += (descriptorSize * index);
    return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE DirectXCommon::GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index)
{
    D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
    handleGPU.ptr += (descriptorSize * index);
    return handleGPU;
}
