#include "MyEngine.h"
#include<algorithm>
//#include"TextureManager.h"

const uint32_t MyEngine::kMaxSRVCount = 512;
uint32_t MyEngine::descriptorSizeSRV = 0;
MyEngine* MyEngine::instance_ = nullptr;

MyEngine* MyEngine::GetInstance()
{

    if (instance_ == nullptr) {
        instance_ = new MyEngine();
    }
    return instance_;

}

void MyEngine::Create(const std::wstring& title, const int32_t clientWidth, const int32_t clientHeight) {


    //誰も捕捉しなかった場合に(Unhandled),補足する関数を登録
    //main関数始まってすぐに登録すると良い
    SetUnhandledExceptionFilter(ExportDump);

    logStream = logFile.CreateLogFile();
    Log(logStream, "CreateLogFile");

    //WindowClassの生成
    wc.Create(title, clientWidth, clientHeight);
    Log(logStream, "CreateWindowClass");

    //InputClassの生成
    input = Input::GetInstance();
    //入力
    input->Initialize(wc);

    //DXGIFactoryの生成
    dxgiFactory.Create();
    Log(logStream, "CreateDXGIFactory");

    //使用するアダプタ(GPU)を決定する
    gpu.SettingGPU(dxgiFactory);
    Log(logStream, "Set GPU");

    //D3D12Deviceの生成
    device = CreateD3D12Device(gpu);
    Log("Complete create D3D12Device!!!\n");//初期化完了のログを出す
    //ファイルへのログ出力
    Log(logStream, "Complete create D3D12Device!!!\n");



#ifdef _DEBUG
    debugError.Create(device);
    Log(logStream, "SetDebugError");
#endif

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

    if (descriptorSizeSRV == 0) {
        descriptorSizeSRV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }
 
  /*  const uint32_t descriptorSizeRYV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);*/

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

#pragma region//InputLayout
    inputLayout.Create();
    Log(logStream, "InputLayout");
#pragma endregion

    //BlendStateの設定を行う

    blendStates.resize(kCountOfBlendMode);
    for (int i = 0; i < blendStates.size(); ++i) {
        blendStates[i].Create(i);
    }

    Log(logStream, "SetBlendState");


    rasterizerStates.resize(3);
    //RasterizerStateの設定を行う
    rasterizerStates[kCullModeNone].Create(kCullModeNone, kFillModeSolid);//ソリッドモードカリングなし
    rasterizerStates[kCullModeFront].Create(kCullModeFront, kFillModeSolid);//ソリッドモード裏面
    rasterizerStates[kCullModeBack].Create(kCullModeBack, kFillModeSolid);//ソリッドモード表面
    //rasterizerStates[0].Create(kCullModeNone, kFillModeWireframe);//ワイヤーフレームモード
    Log(logStream, "SetRasterizerState");

#pragma region//ShaderをCompileする
    dxcCompiler.ShaderSetting();
    Log(logStream, "CompileShader");
#pragma endregion

#pragma region //DepthStencilStateの設定
    depthStencil.Create();
    Log(logStream, "Create depthStencilDesc");
#pragma endregion

    //PSOを生成する
    pso[0].Create(
        rootSignature,
        inputLayout,
        dxcCompiler,
        blendStates[kBlendModeNone],//ブレンドしない
        rasterizerStates[kCullModeBack],//後ろをカリング
        depthStencil,
        device);

    pso[1].Create(
        rootSignature,
        inputLayout,
        dxcCompiler,
        blendStates[kBlendModeNormal],//ブレンドする
        rasterizerStates[kCullModeBack],//後ろをカリング
        depthStencil,
        device);

    pso[2].Create(
        rootSignature,
        inputLayout,
        dxcCompiler,
        blendStates[kBlendModeAdd],//ブレンドしない
        rasterizerStates[kCullModeBack],//描画
        depthStencil,
        device);

    pso[3].Create(
        rootSignature,
        inputLayout,
        dxcCompiler,
        blendStates[kBlendModeSubtract],
        rasterizerStates[kCullModeBack],//描画
        depthStencil,
        device);

    pso[4].Create(
        rootSignature,
        inputLayout,
        dxcCompiler,
        blendStates[kBlendModeMultiply],
        rasterizerStates[kCullModeBack],//描画
        depthStencil,
        device);

    pso[5].Create(
        rootSignature,
        inputLayout,
        dxcCompiler,
        blendStates[kBlendModeScreen],
        rasterizerStates[kCullModeBack],//描画
        depthStencil,
        device);

    Log(logStream, "CreatePSO");


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

#pragma region//平行光源用のResourceを作成する
    directionalLightResource = CreateBufferResource(device, sizeof(DirectionalLight));

    //書き込むためのアドレスを取得
    directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));
    //デフォルト値はとりあえず以下のようにしておく   
    directionalLightData->color = { 1.0f,1.0f,1.0f,1.0f };
    directionalLightData->direction = { 0.0f,1.0f,0.0f };//向きは正規化する
    directionalLightData->intensity = 1.0f;

#pragma endregion

    //ViewportとScissor(シザー)
    viewport = CreateViewport(static_cast<float>(wc.GetClientWidth()), static_cast<float>(wc.GetClientHeight()));
    scissorRect = CreateScissorRect(wc.GetClientWidth(), wc.GetClientHeight());
    Log(logStream, "ViewportAndScissor");

    modelConfig_ = {
        &commandList,
        &rootSignature,
        directionalLightResource,
    };

#ifdef _DEBUG
    //ImGuiの初期化。
    imGuiClass.Initialize(wc, device.Get(), swapChainClass, rtvClass, srvDescriptorHeap);
    Log(logStream, "InitImGui");
#endif

    //ファイルへのログ出力
    Log(logStream, "LoopStart");

}

void MyEngine::Update() {

    //キーボード情報の取得開始
    input->Update();

#ifdef _DEBUG

    //ImGuiにここからフレームが始まる旨を伝える
    imGuiClass.FrameStart();
#endif
}

void MyEngine::PreCommandSet(Vector4& color) {
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

    barrier.SettingBarrier(swapChainResources[backBufferIndex], commandList.GetComandList());

    //2.描画用のRTVとDSVを設定する
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    commandList.GetComandList()->OMSetRenderTargets(1, &rtvClass.GetHandle(backBufferIndex), false, &dsvHandle);
    //3.指定した色で画面全体をクリアする
    float clearColor[] = { color.x,color.y,color.z,color.w };//青っぽい色。RGBAの順
    commandList.GetComandList()->ClearRenderTargetView(rtvClass.GetHandle(backBufferIndex), clearColor, 0, nullptr);

    //指定した深度で画面全体をクリアする
    commandList.GetComandList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    //描画用のDescriptorHeapの設定
    ID3D12DescriptorHeap* descriptorHeaps[] = { srvDescriptorHeap.Get() };
    commandList.GetComandList()->SetDescriptorHeaps(1, descriptorHeaps);

    //PreDrawの一部が共通になったためここに移動
    commandList.GetComandList()->RSSetViewports(1, &viewport);//Viewportを設定
    commandList.GetComandList()->RSSetScissorRects(1, &scissorRect);//Scirssorを設定

};

void MyEngine::PostCommandSet() {

#ifdef _DEBUG
    //諸々の描画処理が終了下タイミングでImGuiの描画コマンドを積む
    imGuiClass.DrawImGui(commandList);

#endif // _DEBUG

    //画面に書く処理は終わり、画面に移すので、状態を遷移
    barrier.Transition();

    //TransitionBarrierを張る
    commandList.GetComandList()->ResourceBarrier(1, &barrier.GetBarrier());

    //4.コマンドリストの内容を確定させる。全てのコマンドを詰んでから　Closesすること。
    HRESULT hr = commandList.GetComandList()->Close();
    assert(SUCCEEDED(hr));
    Log(logStream, "CloseCommandList");

    //5.GPUにコマンドリストの実行を行わせる
    ID3D12CommandList* commandLists[] = { commandList.GetComandList().Get() };
    commandQueue.GetCommandQueue()->ExecuteCommandLists(1, commandLists);
    //6.GPUとOSに画面の交換を行うよう通知する
    swapChainClass.GetSwapChain()->Present(1, 0);

    //画面の更新が終わった直後GPUにシグナルを送る
    //Fenceの値を更新
    fence.AddValue();

    //GPUがここまでたどり着いた時、Fenceの値を指定した値に代入するようにSignalを送る
    fence.SendSignal(commandQueue);

    //Fenceの値が指定したSignal値にたどり着いているか確認する GPUの処理を待つ
    fence.CheckValue(fenceEvent);

    //7.次のフレーム用のコマンドリストを準備
    commandList.PrepareCommand();

#pragma endregion

};

void MyEngine::End() {

#ifdef _DEBUG
    //ImGuiの終了処理 ゲームループが終わったら行う
    imGuiClass.ShutDown();
#endif

#pragma region //解放処理

    CloseHandle(fenceEvent.GetEvent());
    wc.Finalize();

#pragma endregion



    //TextureManager::GetInstance()->Finalize();
}

//ここでBlenModeを変更する
void MyEngine::SetBlendMode(uint32_t blendMode) {

    commandList.GetComandList()->SetPipelineState(pso[blendMode].GetGraphicsPipelineState(PSO::PSOType::TRIANGLE).Get());//PSOを設定

}