#include "DirectXCommon.h"
#include"Log.h"
#include "Viewport.h"
#include "ScissorRect.h"
#include<cassert>

#include"SrvDescriptorHeap.h"
#include"RtvDescriptorHeap.h"
#include"DsvDescriptorHeap.h"

#include "PostProcessManager/PostProcessManager.h"

#include"DebugUI.h"
#include"Input.h"

using namespace Microsoft::WRL;
ComPtr<ID3D12Device> DirectXCommon::device = nullptr;
std::unique_ptr< DxcCompiler> DirectXCommon::dxcCompiler = nullptr;

DirectXCommon::~DirectXCommon()
{

    renderTexture_->Clear();

    renderTextureForSprite_->Clear();

    if (depthTextureData_.depthStencilResource) {
        depthTextureData_.depthStencilResource.Reset();
    }

    dxcCompiler.reset();

    for (auto& resource : swapChainResources) {
        resource.Reset();
    }

    commandList_.reset();
    device.Reset();
}

void DirectXCommon::Finalize()
{
    LogFile::Log("DirectXCommon Finalize\n");
}

void DirectXCommon::PreInitialize(Window& window)
{
    assert(&window);

    window_ = &window;

    InitializeDevice();
    InitializeCommand();
    CreateSwapChain();
    CreateDepthBuffer();
    //コマンドリストのセット
    barrier.SetCommandList(commandList_->Get());

}
void DirectXCommon::PostInitialize()
{
    InitializeFence();
    InitializeViewPort();
    ScissorRectSetting();
    CreateDXCCompiler();

}
void DirectXCommon::CreateDepthStencilResourceSRV(SrvDescriptorHeap* srvDescriptorHeap)
{
    D3D12_SHADER_RESOURCE_VIEW_DESC depthTextureSrvDesc{};
    depthTextureSrvDesc.Format = DXGI_FORMAT_R32_FLOAT;
    depthTextureSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    depthTextureSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    depthTextureSrvDesc.Texture2D.MipLevels = 1;
    depthTextureData_.srvIndex = srvDescriptorHeap->Allocate();
    depthTextureData_.srvHandleCPU = srvDescriptorHeap->GetCPUDescriptorHandle(depthTextureData_.srvIndex);
    depthTextureData_.srvHandleGPU = srvDescriptorHeap->GetGPUDescriptorHandle(depthTextureData_.srvIndex);
    DirectXCommon::GetDevice()->CreateShaderResourceView(depthTextureData_.depthStencilResource.Get(), &depthTextureSrvDesc, depthTextureData_.srvHandleCPU);
    depthTextureData_.depthStencilResource->SetName(L"depthTextureData_depthStencilResource");
    LogFile::Log("Rendertexture : DepthTextureResource : CreateShaderResourceView\n");

}

void DirectXCommon::RenderTexturePreDraw(DsvDescriptorHeap* dsvDescriptorHeap)
{

    auto* commanList = commandList_->Get();

    barrier.SettingBarrier(
        depthTextureData_.depthStencilResource.Get(),
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        D3D12_RESOURCE_STATE_DEPTH_WRITE);

    auto& renderTextureDataNormal = renderTexture_->GetRenderTextureData(RenderTexture::kNormal0);
    auto& renderTextureDataThermography = renderTexture_->GetRenderTextureData(RenderTexture::kThermography);
    auto& renderTextureDataID = renderTexture_->GetRenderTextureData(RenderTexture::kObjectID);

    // 3つのレンダーターゲットハンドルを入れる配列を用意
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[3];

    // SV_TARGET0 用 (メインカラー)
    rtvHandles[0] = renderTextureDataNormal.rtvHandleCPU;
    // SV_TARGET1 用 (温度バッファ) 
    rtvHandles[1] = renderTextureDataThermography.rtvHandleCPU;
    //SV_TARGET2 用 (IDバッファ)
    rtvHandles[2] = renderTextureDataID.rtvHandleCPU;

    //TransitionBarrierの設定
    barrier.SettingBarrierSRVforRTV(renderTextureDataNormal.resource);
    barrier.SettingBarrierSRVforRTV(renderTextureDataThermography.resource);
    barrier.SettingBarrierSRVforRTV(renderTextureDataID.resource);

    //描画用のRTVとDSVを設定する 
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
    // 第1引数を 3 に変更して3つのRTVを同時にバインド
    commanList->OMSetRenderTargets(3, rtvHandles, false, &dsvHandle);

    //指定した色で画面全体をクリアする
    Vector4 color = renderTexture_->GetColor();
    float clearColor[] = { color.x,color.y,color.z,color.w };//青っぽい色。RGBAの順
    commanList->ClearRenderTargetView(renderTextureDataNormal.rtvHandleCPU, clearColor, 0, nullptr);

    //指定した深度で画面全体をクリアする
    commanList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    //温度バッファ(Index 1)のクリア処理を追加
    D3D12_CPU_DESCRIPTOR_HANDLE rtvTemp = renderTexture_->GetRenderTextureData(RenderTexture::kThermography).rtvHandleCPU;
    commanList->ClearRenderTargetView(rtvTemp, clearColor, 0, nullptr);

    // IDバッファ(Index 2)のクリア処理
    commanList->ClearRenderTargetView(renderTextureDataID.rtvHandleCPU, clearColor, 0, nullptr);
}

void DirectXCommon::VeiwPortAndScissorRect()

{
    auto* commanList = commandList_->Get();
    //ビューポート領域の設定
    commanList->RSSetViewports(1, &viewport);//Viewportを設定
    //シザー矩形の設定
    commanList->RSSetScissorRects(1, &scissorRect);//Scirssorを設定
}


void DirectXCommon::DrawRenderTexture(RtvDescriptorHeap* rtvDescriptorHeap)
{
    auto* ppm = PostProcessManager::GetInstance();

    PostProcessManager::Layer modelLayer = PostProcessManager::kModel;
    //Executeまでにこれをセットしておく
    ppm->SetRenderTexture(renderTexture_.get(), modelLayer);
    ppm->ClearEffects(modelLayer);

    ppm->AddEffect(PSO::kEffectGrayScale, modelLayer);
    ppm->AddEffect(PSO::kEffectDepthBasedOutline, modelLayer);
    ppm->AddEffect(PSO::kEffectLuminanceBasedOutline, modelLayer);
    ppm->AddEffect(PSO::kEffectBoxFilter, modelLayer);
    ppm->AddEffect(PSO::kEffectGaussianFilter, modelLayer);
    ppm->AddEffect(PSO::kEffectRadialBlur, modelLayer);
    ppm->AddEffect(PSO::kEffectVignette, modelLayer);
    //ppm.AddEffect(PSO::kEffectRandom);
    ppm->AddEffect(PSO::kEffectThermography, modelLayer);
    ppm->AddEffect(PSO::kEffectDissolve, modelLayer);

    PostProcessManager::Layer spriteLayer = PostProcessManager::kSprite;
    ppm->SetRenderTexture(renderTextureForSprite_.get(), spriteLayer);
    ppm->ClearEffects(spriteLayer);
    ppm->AddEffect(PSO::kEffectThermography, spriteLayer);
    ppm->AddEffect(PSO::kEffectDissolve, spriteLayer);

    //描画先を画面(バックバッファ)のRTVにする
    // バックバッファは PreDraw で既に RENDER_TARGET 状態になっています
    UINT backBufferIndex = swapChainClass.GetSwapChain()->GetCurrentBackBufferIndex();

    auto backBufferRTV = rtvDescriptorHeap->GetCPUDescriptorHandle(backBufferIndex);

    ppm->Execute(modelLayer, backBufferRTV, &barrier, depthTextureData_.srvIndex, kBlendModeMultiply);


}

void DirectXCommon::DrawRenderTextureForSprite(RtvDescriptorHeap* rtvDescriptorHeap)
{   
    auto* ppm = PostProcessManager::GetInstance();
    //描画先を画面(バックバッファ)のRTVにする
// バックバッファは PreDraw で既に RENDER_TARGET 状態になっています
    UINT backBufferIndex = swapChainClass.GetSwapChain()->GetCurrentBackBufferIndex();

    auto backBufferRTV = rtvDescriptorHeap->GetCPUDescriptorHandle(backBufferIndex);
    PostProcessManager::Layer spriteLayer = PostProcessManager::kSprite;
    // ② スプライトのエフェクトをバックバッファに「上書き（アルファブレンド）」出力
    ppm->Execute(spriteLayer, backBufferRTV, &barrier, depthTextureData_.srvIndex, kBlendModeNormal);

}

void DirectXCommon::RenderTexturePostDraw()
{

    auto* commanList = commandList_->Get();

    if (Input::IsTriggerMouse(0)) {
        Vector2Int pos = Input::GetCursorPositionInt();
        renderTexture_->CopyClickPixelCommand(pos.x, pos.y);
    }

    barrier.SettingBarrier(
        depthTextureData_.depthStencilResource.Get(),
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    auto& renderTextureData = renderTexture_->GetRenderTextureData(RenderTexture::kNormal0);
    barrier.SettingBarrierRTVforSRV(renderTextureData.resource);

    auto& renderTextureDataThermography = renderTexture_->GetRenderTextureData(RenderTexture::kThermography);
    barrier.SettingBarrierRTVforSRV(renderTextureDataThermography.resource);

    // ★追加: ID用テクスチャのバリアを元に戻す
    auto& renderTextureDataID = renderTexture_->GetRenderTextureData(RenderTexture::kObjectID);
    barrier.SettingBarrierRTVforSRV(renderTextureDataID.resource);
}


void DirectXCommon::SettingIdTextureBarrierPost()
{

    auto& renderTextureDataID = renderTexture_->GetRenderTextureData(RenderTexture::kObjectID);

    barrier.SettingBarrier(
        renderTextureDataID.resource,
        D3D12_RESOURCE_STATE_COPY_SOURCE,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
    );
}

void DirectXCommon::SettingIdTextureBarrierPre()
{
    auto& renderTextureDataID = renderTexture_->GetRenderTextureData(RenderTexture::kObjectID);
    barrier.SettingBarrier(
        renderTextureDataID.resource,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        D3D12_RESOURCE_STATE_COPY_SOURCE);
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

    auto* commanList = commandList_->Get();

    //TransitionBarrierの設定
    barrier.SettingBarrier(
        swapChainResources[backBufferIndex],
        D3D12_RESOURCE_STATE_PRESENT,
        D3D12_RESOURCE_STATE_RENDER_TARGET);

    commanList->OMSetRenderTargets(1, &rtvClass.GetHandle(backBufferIndex), false, nullptr);
    //3.指定した色で画面全体をクリアする
    Vector4 color = renderTexture_->GetColor();
    float clearColor[] = { color.x,color.y,color.z,color.w };//青っぽい色。RGBAの順
    commanList->ClearRenderTargetView(rtvClass.GetHandle(backBufferIndex), clearColor, 0, nullptr);

}

void DirectXCommon::PostDraw()
{
    //1.これから書き込むバックバッファのインデックスを取得
    UINT backBufferIndex = swapChainClass.GetSwapChain()->GetCurrentBackBufferIndex();

    auto* commanList = commandList_->Get();

    //TransitionBarrierの設定
    barrier.SettingBarrier(
        swapChainResources[backBufferIndex],
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PRESENT);

    //4.コマンドリストの内容を確定させる。全てのコマンドを詰んでから　Closesすること。
    commanList->Close();

    //5.GPUにコマンドリストの実行を行わせる
    ID3D12CommandList* commandLists[] = { commanList };
    commandQueue.GetCommandQueue()->ExecuteCommandLists(1, commandLists);
    //6.GPUとOSに画面の交換を行うよう通知する
    swapChainClass.GetSwapChain()->Present(1, 0);

    //画面の更新が終わった直後GPUにシグナルを送る
    fence.SendSignal(commandQueue);

}

void DirectXCommon::PrepareCommand()
{    //7.次のフレーム用のコマンドリストを準備
    commandList_->PrepareCommand();
}

void DirectXCommon::RenderTextureForSpritePreDraw()
{
    auto* commanList = commandList_->Get();

    auto& renderTextureDataNormal = renderTextureForSprite_->GetRenderTextureData(RenderTexture::kNormal0);
    auto& renderTextureDataThermography = renderTextureForSprite_->GetRenderTextureData(RenderTexture::kThermography);

    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];
    rtvHandles[0] = renderTextureDataNormal.rtvHandleCPU;
    rtvHandles[1] = renderTextureDataThermography.rtvHandleCPU;

    // SRVからRTVへバリア遷移
    barrier.SettingBarrierSRVforRTV(renderTextureDataNormal.resource);
    barrier.SettingBarrierSRVforRTV(renderTextureDataThermography.resource);

    // スプライト描画用のRTVをセット (深度バッファは不要なら nullptr)
    commanList->OMSetRenderTargets(2, rtvHandles, false, nullptr);

    //3.指定した色で画面全体をクリアする
    float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    commanList->ClearRenderTargetView(rtvHandles[0], clearColor, 0, nullptr);
    commanList->ClearRenderTargetView(rtvHandles[1], clearColor, 0, nullptr);

}

void DirectXCommon::RenderTextureForSpritePostDraw()
{// RTVからSRVへバリア遷移（これでPostProcessManagerがテクスチャとして読み込めるようになる）
    auto& renderTextureDataNormal = renderTextureForSprite_->GetRenderTextureData(RenderTexture::kNormal0);
    auto& renderTextureDataThermography = renderTextureForSprite_->GetRenderTextureData(RenderTexture::kThermography);

    barrier.SettingBarrierRTVforSRV(renderTextureDataNormal.resource);
    barrier.SettingBarrierRTVforSRV(renderTextureDataThermography.resource); 
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
    LogFile::Log("CreateDXGIFactory\n");

    //使用するアダプタ(GPU)を決定する
    gpu.SettingGPU(dxgiFactory);
    LogFile::Log("Set GPU\n");

    //D3D12Deviceの生成
    device = CreateD3D12Device(gpu);
    //ファイルへのログ出力
    LogFile::Log("Complete create D3D12Device!!!\n");//初期化完了のログを出す

#ifdef _DEBUG
    debugError.Create(device);
    LogFile::Log("SetDebugError\n");
#endif

}

void DirectXCommon::InitializeCommand()
{
    commandList_ = std::make_unique<CommandList>();
    //コマンドリストの生成
    commandList_->Create();
    LogFile::Log("Create CommandList\n");

    //コマンドキューの生成
    commandQueue.Create(device);
    LogFile::Log("Create CommandQueue\n");

}

void DirectXCommon::CreateSwapChain()
{

    //スワップチェインの生成
//画面の幅。ウィンドウのクライアント領域を同じものにしておく
    swapChainClass.Create(
        *window_,
        dxgiFactory.GetDigiFactory(),
        commandQueue.GetCommandQueue());
    LogFile::Log("CreateSwapChain\n");

#pragma region//SwapChainからResourceを引っ張ってくる
    //SwapChainからResourceを引っ張ってくる
    swapChainClass.GetBuffer(0, swapChainResources[0]);
    swapChainClass.GetBuffer(1, swapChainResources[1]);
    LogFile::Log("Pull Resource from SwapChain\n");

#pragma endregion

}

void DirectXCommon::CreateDepthBuffer()
{
    //stencileTextureResourceの作成 

    depthTextureData_.depthStencilResource = CreateDepthStencileTextureResource(window_->GetClientWidth(), window_->GetClientHeight());

    LogFile::Log("CreateDepthBuffer\n");

}

void DirectXCommon::InitializeRenderTargetView(RtvDescriptorHeap* rtvDescriptorHeap)
{
    //RTVを作る
    rtvClass.Create(swapChainResources, rtvDescriptorHeap);
    LogFile::Log("CreateRTV\n");

}

void DirectXCommon::InitializeDepthStencilView(DsvDescriptorHeap* dsvDescriptorHeap)
{
    //DSVの設定 DepthStencilView
    dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;//基本的にはResourceに合わせる。
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;//2dTexture
    // DSVHeapの先頭にDSVを作る
    device->CreateDepthStencilView(depthTextureData_.depthStencilResource.Get(), &dsvDesc, dsvDescriptorHeap->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart());

    LogFile::Log("Initialize DepthStencilView\n");
}

void DirectXCommon::UpdateGameScreen(SrvDescriptorHeap* srvDescriptorHeap)
{    //一旦ここでゲーム画面の描画
    DebugUI::ShowMainViewPort(srvDescriptorHeap, renderTexture_->GetRenderTextureData(RenderTexture::kNormal1).srvIndex);

}

void DirectXCommon::InitializeFence()
{
#pragma region //FenceとEventを生成する
    fence.Create(device);
    LogFile::Log("CreateFence\n");
#pragma endregion
}

void DirectXCommon::InitializeViewPort()
{
    viewport = CreateViewport(static_cast<float>(window_->GetClientWidth()), static_cast<float>(window_->GetClientHeight()));

    LogFile::Log("InitializeViewPort\n");
}

void DirectXCommon::ScissorRectSetting()
{
    //ViewportとScissor(シザー)
    scissorRect = CreateScissorRect(window_->GetClientWidth(), window_->GetClientHeight());
    LogFile::Log("Viewport And Scissor\n");
}

void DirectXCommon::CreateDXCCompiler()
{
    dxcCompiler = std::make_unique<DxcCompiler>();
    dxcCompiler->Initialize();
    dxcCompiler->ShaderSetting();
    LogFile::Log("Init DxcCompiler\n");

}


void DirectXCommon::InitializeRenderTexture(RtvDescriptorHeap* rtvDescriptorHeap, SrvDescriptorHeap* srvDescriptorHeap)
{
    renderTexture_ = std::make_unique<RenderTexture>();
    renderTexture_->SetCommandListAndSrvDescriptorHeap(commandList_->Get(), srvDescriptorHeap);
    renderTexture_->Create(rtvDescriptorHeap);

    //スプライト用のレンダーテクスチャを作成
    renderTextureForSprite_ = std::make_unique<RenderTexture>();
    renderTextureForSprite_->SetCommandListAndSrvDescriptorHeap(commandList_->Get(), srvDescriptorHeap);
    renderTextureForSprite_->Create(rtvDescriptorHeap);
    renderTextureForSprite_->SetRenderTargetClearValue({ 0.0f,0.0f,0.0f,0.0f });
}

void DirectXCommon::UpdateRenderTexture(SrvDescriptorHeap* srvDescriptorHeap)
{

#ifdef USE_IMGUI
    ImGui::Begin("PostEffect Viewer");

    // 例：表示したいSRVのインデックス番号
    DebugUI::CheckSRVTexture(depthTextureData_.srvIndex, srvDescriptorHeap);
    for (auto& textureData : renderTexture_->GetRenderTextureDatas()) {
        DebugUI::CheckSRVTexture(textureData.srvIndex, srvDescriptorHeap);
    }
    for (auto& textureData : renderTextureForSprite_->GetRenderTextureDatas()) {
        DebugUI::CheckSRVTexture(textureData.srvIndex, srvDescriptorHeap);
    }
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

    resource->SetName(L"DirectXCommon_BufferResource");

    return nullptr;


};


ComPtr<ID3D12Resource> DirectXCommon::CreateReadbackBufferResource(size_t sizeInBytes) {
    ComPtr<ID3D12Resource> resource = nullptr;

    // 1. ヒープの設定をREADBACKに変更
    D3D12_HEAP_PROPERTIES heapProperties{};
    heapProperties.Type = D3D12_HEAP_TYPE_READBACK; // ここが重要

    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resourceDesc.Width = sizeInBytes; // 1ピクセル分(UINTなら4バイト)
    resourceDesc.Height = 1;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.MipLevels = 1;
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    // 2. 初期状態を COPY_DEST にする
    if (SUCCEEDED(device->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_COPY_DEST, // ここが重要
        nullptr,
        IID_PPV_ARGS(&resource)))) {
        return resource;
    }
    resource->SetName(L"DirectXCommon_ReadbackBufferResource");
    return nullptr;
}

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

    resource->SetName(L"DirectXCommon_TextureResource");
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
    resource->SetName(L"DirectXCommon_DepthStencile_TextureResource");
    return resource;
}

[[nodiscard]]
Microsoft::WRL::ComPtr<ID3D12Resource> DirectXCommon::UploadTextureData(
    ID3D12GraphicsCommandList* commandList,
    const Microsoft::WRL::ComPtr<ID3D12Resource>& texture,
    const DirectX::ScratchImage& mipImages
)
{
    std::vector<D3D12_SUBRESOURCE_DATA>subresources;
    DirectX::PrepareUpload(device.Get(), mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
    uint64_t intermediateSize = GetRequiredIntermediateSize(texture.Get(), 0, UINT(subresources.size()));
    Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = CreateBufferResource(intermediateSize);//中間リソース
    intermediateResource->SetName(L"texture:intermediateResource");
    UpdateSubresources(commandList, texture.Get(), intermediateResource.Get(), 0, 0, UINT(subresources.size()), subresources.data());
    //Textureへの転送後は利用できるよう,D3D12_RESOURCE_STATE_COPY_DESTからRESOURCE_STATE_GENERIC_READへResourceStateを変更する
    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = texture.Get();
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;//これに変更
    commandList->ResourceBarrier(1, &barrier);
    return intermediateResource;
}

