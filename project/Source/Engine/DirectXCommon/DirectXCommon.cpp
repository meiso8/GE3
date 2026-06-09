#include "DirectXCommon.h"
#include"Log.h"
#include "Viewport.h"
#include "ScissorRect.h"
#include<cassert>
#include <thread>
#include"SRVmanager/SrvManager.h"
using namespace Microsoft::WRL;
ComPtr<ID3D12Device> DirectXCommon::device = nullptr;

ComPtr<ID3D12DescriptorHeap> DirectXCommon::rtvDescriptorHeap = nullptr;
ComPtr<ID3D12DescriptorHeap> DirectXCommon::dsvDescriptorHeap = nullptr;

const uint32_t DirectXCommon::kMaxSoundCount = 128;
const uint32_t DirectXCommon::kMaxModelCount = 128;
uint32_t DirectXCommon::descriptorSizeRTV = 0;
uint32_t DirectXCommon::descriptorSizeDSV = 0;


std::unique_ptr< DxcCompiler> DirectXCommon::dxcCompiler = nullptr;
std::unique_ptr<CommandList> DirectXCommon::commandList = nullptr;

float DirectXCommon::deltaTime_ = 1.0f / 60.0f;

DirectXCommon::~DirectXCommon()
{

    if (depthTextureData_.depthStencilResource) {
        depthTextureData_.depthStencilResource.Reset();
    }

    dxcCompiler.reset();
    dsvDescriptorHeap.Reset();

    rtvDescriptorHeap.Reset();

    for (auto& resource : swapChainResources) {
        resource.Reset();
    }

    commandList.reset();
    device.Reset();

}

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
}

void DirectXCommon::CreateDepthStencilResourceSRV()
{
    D3D12_SHADER_RESOURCE_VIEW_DESC depthTextureSrvDesc{};
    depthTextureSrvDesc.Format = DXGI_FORMAT_R32_FLOAT;
    depthTextureSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    depthTextureSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    depthTextureSrvDesc.Texture2D.MipLevels = 1;
    depthTextureData_.srvIndex = SrvManager::Allocate();
    depthTextureData_.srvHandleCPU = SrvManager::GetCPUDescriptorHandle(depthTextureData_.srvIndex);
    depthTextureData_.srvHandleGPU = SrvManager::GetGPUDescriptorHandle(depthTextureData_.srvIndex);
    DirectXCommon::GetDevice()->CreateShaderResourceView(depthTextureData_.depthStencilResource.Get(), &depthTextureSrvDesc, depthTextureData_.srvHandleCPU);
    LogFile::Log("Rendertexture : DepthTextureResource : CreateShaderResourceView");

}

void DirectXCommon::RenderTexturePreDraw()
{

    barrier.SettingBarrier(
        depthTextureData_.depthStencilResource.Get(),
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        D3D12_RESOURCE_STATE_DEPTH_WRITE);

    auto& renderTextureDataNormal = renderTexture_->GetRenderTextureData(RenderTexture::kNormal0);
    auto& renderTextureDataThermography = renderTexture_->GetRenderTextureData(RenderTexture::kThermography);
    // 2つのレンダーターゲットハンドルを入れる配列を用意
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];

    // SV_TARGET0 用 (メインカラー)
    rtvHandles[0] = renderTextureDataNormal.rtvHandleCPU;
    // SV_TARGET1 用 (温度バッファ) 
    rtvHandles[1] = renderTextureDataThermography.rtvHandleCPU;

    //TransitionBarrierの設定
    barrier.SettingBarrierSRVforRTV(renderTextureDataNormal.resource);
    barrier.SettingBarrierSRVforRTV(renderTextureDataThermography.resource);

    //2.描画用のRTVとDSVを設定する 
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    // ★ 第1引数を 2 にし、第3引数を FALSE (連続したメモリ配列として渡す) にする
    commandList->GetCommandList()->OMSetRenderTargets(2, rtvHandles, false, &dsvHandle);
    //commandList->GetCommandList()->OMSetRenderTargets(1, &renderTextureData.rtvHandleCPU, false, &dsvHandle);

    //3.指定した色で画面全体をクリアする
    Vector4 color = renderTexture_->GetColor();
    float clearColor[] = { color.x,color.y,color.z,color.w };//青っぽい色。RGBAの順
    commandList->GetCommandList()->ClearRenderTargetView(renderTextureDataNormal.rtvHandleCPU, clearColor, 0, nullptr);

    //指定した深度で画面全体をクリアする
    commandList->GetCommandList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    // ★ 2. 温度バッファ(Index 1)のクリア処理を追加！
    D3D12_CPU_DESCRIPTOR_HANDLE rtvTemp = renderTexture_->GetRenderTextureData(RenderTexture::kThermography).rtvHandleCPU;
    // 温度の初期値は「0.0（熱源なし）」にリセットしたいので、すべて 0.0f にします
    //renderTexture_->GetColor();

    commandList->GetCommandList()->ClearRenderTargetView(rtvTemp, clearColor, 0, nullptr);


    SrvManager::PreDraw();

    //ビューポート領域の設定
    commandList->GetCommandList()->RSSetViewports(1, &viewport);//Viewportを設定
    //シザー矩形の設定
    commandList->GetCommandList()->RSSetScissorRects(1, &scissorRect);//Scirssorを設定


}
#include "PostProcessManager/PostProcessManager.h"

void DirectXCommon::DrawRenderTexture()
{

    PostProcessManager ppm;
    ppm.ClearEffects();
    ppm.AddEffect(PSO::kEffectGrayScale);
    ppm.AddEffect(PSO::kEffectDepthBasedOutline);
    ppm.AddEffect(PSO::kEffectLuminanceBasedOutline);
    ppm.AddEffect(PSO::kEffectBoxFilter);
    ppm.AddEffect(PSO::kEffectGaussianFilter);
    ppm.AddEffect(PSO::kEffectRadialBlur);
    ppm.AddEffect(PSO::kEffectVignette);
    //ppm.AddEffect(PSO::kEffectRandom);
    ppm.AddEffect(PSO::kEffectThermography);
    ppm.AddEffect(PSO::kEffectDissolve);

    //描画先を画面(バックバッファ)のRTVにする
    // バックバッファは PreDraw で既に RENDER_TARGET 状態になっています
    UINT backBufferIndex = swapChainClass.GetSwapChain()->GetCurrentBackBufferIndex();
    auto backBufferRTV = GetRTVCPUDescriptorHandle(backBufferIndex);

    ppm.Execute(renderTexture_, backBufferRTV, &barrier, depthTextureData_.srvIndex, kBlendModeMultiply);
}

void DirectXCommon::RenderTexturePostDraw()
{

    barrier.SettingBarrier(depthTextureData_.depthStencilResource.Get(),
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    auto& renderTextureData = renderTexture_->GetRenderTextureData(RenderTexture::kNormal0);
    barrier.SettingBarrierRTVforSRV(renderTextureData.resource);

    auto& renderTextureDataThermography = renderTexture_->GetRenderTextureData(RenderTexture::kThermography);
    barrier.SettingBarrierRTVforSRV(renderTextureDataThermography.resource);

    LogFile::Log("Rendertexture : PosDraw : SettingBarrier");
}

void DirectXCommon::SetRenderTextureCamera(Camera* camera)
{
    renderTexture_->SetCamera(camera);
}

void DirectXCommon::PreDraw()
{

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
    barrier.SettingBarrier(swapChainResources[backBufferIndex],
        D3D12_RESOURCE_STATE_PRESENT,
        D3D12_RESOURCE_STATE_RENDER_TARGET);


    commandList->GetCommandList()->OMSetRenderTargets(1, &rtvClass.GetHandle(backBufferIndex), false, nullptr);
    //3.指定した色で画面全体をクリアする
    Vector4 color = renderTexture_->GetColor();
    float clearColor[] = { color.x,color.y,color.z,color.w };//青っぽい色。RGBAの順
    commandList->GetCommandList()->ClearRenderTargetView(rtvClass.GetHandle(backBufferIndex), clearColor, 0, nullptr);

    SrvManager::PreDraw();

    //ビューポート領域の設定
    commandList->GetCommandList()->RSSetViewports(1, &viewport);//Viewportを設定
    //シザー矩形の設定
    commandList->GetCommandList()->RSSetScissorRects(1, &scissorRect);//Scirssorを設定


}

void DirectXCommon::PostDraw()
{
    //1.これから書き込むバックバッファのインデックスを取得
    UINT backBufferIndex = swapChainClass.GetSwapChain()->GetCurrentBackBufferIndex();

    //TransitionBarrierの設定
    barrier.SettingBarrier(swapChainResources[backBufferIndex],
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PRESENT);

    //4.コマンドリストの内容を確定させる。全てのコマンドを詰んでから　Closesすること。
    HRESULT hr = commandList->GetCommandList()->Close();
    assert(SUCCEEDED(hr));
    /*  LogFile::Log("CloseCommandList");*/

      //5.GPUにコマンドリストの実行を行わせる
    ID3D12CommandList* commandLists[] = { commandList->GetCommandList().Get() };
    commandQueue.GetCommandQueue()->ExecuteCommandLists(1, commandLists);
    //6.GPUとOSに画面の交換を行うよう通知する
    swapChainClass.GetSwapChain()->Present(1, 0);

    //画面の更新が終わった直後GPUにシグナルを送る
    fence.SendSignal(commandQueue);

    UpdateFixFPS();

    //7.次のフレーム用のコマンドリストを準備
    commandList->PrepareCommand();

}

void DirectXCommon::EndFrame()
{


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

    depthTextureData_.depthStencilResource = CreateDepthStencileTextureResource(window_->GetClientWidth(), window_->GetClientHeight());

    LogFile::Log("CreateDepthBuffer");

}

void DirectXCommon::DescriptorHeapSettings()
{
    //DescriptorSizeを取得しておく
    descriptorSizeRTV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    descriptorSizeDSV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

    //DescriptorHeapを生成する
    if (rtvDescriptorHeap == nullptr) {
        rtvDescriptorHeap = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 64, false);
        LogFile::Log("Create RTV DescriptorHeap");
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
    dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;//基本的にはResourceに合わせる。
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;//2dTexture
    // DSVHeapの先頭にDSVを作る
    device->CreateDepthStencilView(depthTextureData_.depthStencilResource.Get(), &dsvDesc, dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

    LogFile::Log("InitializeDepthStencilView");
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

    LogFile::Log("InitializeViewPort");
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

void DirectXCommon::InitializeFixFPS()
{
    reference_ = std::chrono::steady_clock::now();
    LogFile::Log("InitializeFixFPS");
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


    // スリープを抜けた現在時間を確定させる
    std::chrono::steady_clock::time_point frame_end = std::chrono::steady_clock::now();

    // 実際に1フレームにかかった時間を「秒単位のfloat」で取得
    deltaTime_ = std::chrono::duration<float>(frame_end - reference_).count();

    //現在の時間を記録する
    reference_ = frame_end;

    std::string message = "DeltaTime : " + std::to_string(deltaTime_);
    LogFile::Log(message.c_str());
    LogFile::Log("UpdateFixFPS");

}

void DirectXCommon::InitializeRenderTexture()
{
    renderTexture_ = RenderTexture::GetInstance();
    renderTexture_->Create();
}

void DirectXCommon::UpdateRenderTexture()
{
    renderTexture_->Update();
#ifdef USE_IMGUI
    ImGui::Begin("SRVTexture");
    // 例：表示したいSRVのインデックス番号
    // （テクスチャを読み込んだ時のインデックスや、RenderTextureのsrvIndexなど）
    // ImGui::Imageに渡すために ImTextureID (void* 型) にキャストする
    ImTextureID texID = reinterpret_cast<ImTextureID>(depthTextureData_.srvHandleGPU.ptr);
    ImTextureID texIDTemp = reinterpret_cast<ImTextureID>(renderTexture_->GetRenderTextureData(RenderTexture::kThermography).srvHandleGPU.ptr);

    // 画像の表示 (引数: テクスチャID, 表示サイズ(横, 縦))
    ImGui::Image(texID, ImVec2(128.0f, 72.0f));
    ImGui::Image(texIDTemp, ImVec2(128.0f, 72.0f));
    ImGui::End();
#endif
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

Microsoft::WRL::ComPtr<ID3D12Resource> DirectXCommon::CreateRenderTextureResource(uint32_t width, uint32_t height, DXGI_FORMAT format, const Vector4& clearColor)
{

    //1. metadataを基にResourceの設定
    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Width = width;//Textureの値
    resourceDesc.Height = height;//Textureの高さ
    resourceDesc.MipLevels = 1;//mipmapの数
    resourceDesc.DepthOrArraySize = 1;//奥行き　or 配列Textureの配列数
    resourceDesc.Format = format;//TextureのFormat
    resourceDesc.SampleDesc.Count = 1;//サンプリングカウント。1固定。
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;//2次元
    resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;//RenderTarget

    //2.利用するHeapの設定
    D3D12_HEAP_PROPERTIES heapProperties{};
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;//GPUで処理するために書き換え

    D3D12_CLEAR_VALUE clearValue;
    clearValue.Format = format;
    clearValue.Color[0] = clearColor.x;
    clearValue.Color[1] = clearColor.y;
    clearValue.Color[2] = clearColor.z;
    clearValue.Color[3] = clearColor.w;

    //3.Resourceを生成する
    Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
    HRESULT hr = device->CreateCommittedResource(
        &heapProperties,//Heapの設定
        D3D12_HEAP_FLAG_NONE,//Heapの特殊な設定。特になし
        &resourceDesc,//Resourceの設定
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,// SRVとする
        &clearValue,//Clear最適地。
        IID_PPV_ARGS(&resource));//ポインタのポインタ

    assert(SUCCEEDED(hr));

    return resource;
}


Microsoft::WRL::ComPtr<ID3D12Resource> DirectXCommon::CreateDepthStencileTextureResource(
    int32_t width,
    int32_t height) {
    //生成するResourceの設定
    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Width = width;//Textureの幅
    resourceDesc.Height = height;//高さ
    resourceDesc.MipLevels = 1;//mipmapの数
    resourceDesc.DepthOrArraySize = 1;//奥行き　or 配列Textureの配列数
    resourceDesc.Format = DXGI_FORMAT_R32_TYPELESS;//DepthStencilとして利用可能なフォーマット
    resourceDesc.SampleDesc.Count = 1;//サンプリングカウント
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;//2次元
    resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;//DepthStencilとして使う通知

    //利用するHeapの設定
    D3D12_HEAP_PROPERTIES heapProperties{};
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;//VRAM上

    //深度値クリア設定
    D3D12_CLEAR_VALUE depthClearValue{};
    depthClearValue.DepthStencil.Depth = 1.0f;//1.0f(最大値)でクリア
    depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;//フォーマット。Resourceと合わせる

    //Resourceの生成
    Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
    HRESULT hr = device->CreateCommittedResource(
        &heapProperties,//Heapの設定
        D3D12_HEAP_FLAG_NONE,//Heapの特殊な設定。特になし。
        &resourceDesc,//Resourceの設定
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,//深度値を最初は使う状態にする
        &depthClearValue,//Clear最適地
        IID_PPV_ARGS(&resource));
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
