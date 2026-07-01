#include "ButtobiEngine.h"

#include"DirectXCommon.h"
#include"TimeManager.h"
#include"Input.h"
#include"VibrateManager.h"
#include"Log.h"
#include"PSO.h"
#include"CrashHandler.h"

#include"SpriteCamera.h"
#include"ModelManager.h"

#include"DrawGrid.h"

#include"JsonFile.h"
#include"Sound.h"

#include"SceneManager.h"
#include"SceneFactory.h"

#include"Application/Loader/ModelFactory.h"
#include"Application/Loader/SoundFactory.h"
#include"Application/Loader/TextureFactory.h"

#include"ObjectManager/ObjectManager.h"
#include"Particle/ParticleEmitter.h"

#include"PostProcessManager/PostProcessManager.h"

#include"DebugUI.h"
#include"DebugCamera.h"
#include"Animation/SkinCluster.h"

void ButtobiEngine::SetCommandListAndSrvDescriptorHeap()
{
    auto* commandList = directXCommon_->GetCommandListClass()->Get();
    auto* srv = srvDescriptorHeap_.get();
    Sprite::SetCommandListAndSrvDescriptorHeap(commandList, srv);
    texture_->SetCommandListAndSrvDescriptorHeap(commandList, srv);
    freeTypeManager_->SetCommandListAndSrvDescriptorHeap(commandList, srv);
    ObjectManager::GetInstance()->SetCommandListAndSrvDescriptorHeap(commandList, srv);
    particleManager_->SetCommandListAndSrvDescriptorHeap(commandList, srv);
}

void ButtobiEngine::Create(const std::wstring& title, const int32_t clientWidth, const int32_t clientHeight) {


    //誰も捕捉しなかった場合に(Unhandled),補足する関数を登録
    //main関数始まってすぐに登録すると良い
    SetUnhandledExceptionFilter(ExportDump);
    //ログファイルの生成
    logFile = std::make_unique<LogFile>();
    //WindowClassの生成
    wc = std::make_unique<Window>(title, clientWidth, clientHeight);
    //InputClassの生成
    input = std::make_unique<Input>();
    HRESULT result = input->Initialize(*wc);
    assert(SUCCEEDED(result));
    //コントローラーの振動管理
    VibrateManager::Initialize();
    //FPSや時間管理の生成
    time_ = std::make_unique<TimeManager>();

    //DirectXCommon
    directXCommon_ = std::make_unique<DirectXCommon>();
    directXCommon_->PreInitialize(*wc);
    LogFile::Log("DirectXCommon PreInitialize");

    //rtvManagerの生成
    rtvDescriptorHeap_ = std::make_unique<RtvDescriptorHeap>();
    //dsvDescriptorHeap_の生成
    dsvDescriptorHeap_ = std::make_unique<DsvDescriptorHeap>();

    directXCommon_->InitializeRenderTargetView(rtvDescriptorHeap_.get());
    LogFile::Log("DirectXCommon Initialize Render Target View\n");
    directXCommon_->InitializeDepthStencilView(dsvDescriptorHeap_.get());
    LogFile::Log("DirectXCommon Initialize dsv Descriptor Heap\n");
    directXCommon_->PostInitialize();
    LogFile::Log("DirectXCommon PostInitialize\n");

    //SRV管理
    srvDescriptorHeap_ = std::make_unique<SrvDescriptorHeap>();

#ifdef USE_IMGUI
    //ImGuiの初期化。
    imGuiClass.Initialize(
        *wc,
        srvDescriptorHeap_.get()
        , directXCommon_->GetDevice().Get(),
        directXCommon_->GetSwapChain(),
        directXCommon_->GetSwapChainRtv()
    );
    LogFile::Log("InitImGui");
#endif

    auto* commandList = directXCommon_->GetCommandListClass()->Get();

    directXCommon_->InitializeRenderTexture(rtvDescriptorHeap_.get(), srvDescriptorHeap_.get());

    directXCommon_->CreateDepthStencilResourceSRV(srvDescriptorHeap_.get());

    auto* pso = PSO::GetInstance();
    pso->CreateALLPSO();
    LogFile::Log("CreatePSO");

#pragma region//LightManager
    //方向ライト管理の作成
    directionalLightManager_ = std::make_unique<DirectionalLightManager>();
    //ポイントライト管理の作成
    pointLightManager_ = ::std::make_unique<PointLightManager>(srvDescriptorHeap_.get());
    //スポットライト管理の作成
    spotLightManager_ = ::std::make_unique<SpotLightManager>(srvDescriptorHeap_.get());
#pragma endregion

    //共通のスプライト
    spriteCommon_ = std::make_unique<SpriteCommon>();
    //PSOの設定と初期化
    spriteCommon_->Initialize(pso->GetRootSignature());

    Sprite::SetCommandListAndSrvDescriptorHeap(
        commandList,
        srvDescriptorHeap_.get()
    );

    //スプライト用カメラ
    SpriteCamera::Initialize(static_cast<float>(wc->GetClientWidth()), static_cast<float>(wc->GetClientHeight()));
    LogFile::Log("Initialize　SpriteCamera");
    //サウンド管理
    Sound::Initialize();
    LogFile::Log("Initialize Sound");

    //テクスチャ管理
    texture_ = std::make_unique<Texture>();
    texture_->Initialize();
    texture_->SetCommandListAndSrvDescriptorHeap(commandList, srvDescriptorHeap_.get());
    //テキストの初期化
    freeTypeManager_ = std::make_unique<FreeTypeManager>();
    freeTypeManager_->SetCommandListAndSrvDescriptorHeap(commandList, srvDescriptorHeap_.get());

    //テスクチャ読み込み
    TextureFactory::Load();
    LogFile::Log("LoadAllTexture");

    //音声の読み込み
    SoundFactory::Load();
    LogFile::Log("LoadAllSound");

    //JsonFileの読み込み
    JsonFile::LoadAllJsonFile();
    LogFile::Log("LoadAllJsonFile");

    //モデル読み込み
    ModelFactory::Load();
    LogFile::Log("LoadAllModel");

    //プリミティブ生成
    primitiveFactory_ = std::make_unique<PrimitiveFactory>();
    primitiveFactory_->CreateAllPrimitive();
    LogFile::Log("CreatePrimitive");

    Skin::SetSrvDescriptorHeap(srvDescriptorHeap_.get());

    tagFactory_ = std::unique_ptr<TagFactory>();
    //タグの作成
    tagFactory_->SetTag();
    LogFile::Log("Create Tag");

    //オブジェクト管理の初期化
    ObjectManager::GetInstance()->Initialize();
    ObjectManager::GetInstance()->SetCommandListAndSrvDescriptorHeap(
        commandList,
        srvDescriptorHeap_.get()
    );
    LogFile::Log("ObjectManager Initialize");

#ifdef _DEVELOP
    //グリット描画
    DrawGrid::Create();
    LogFile::Log("CreateDrawGrid");
#endif

    //パーティクル管理
    particleManager_ = std::make_unique<ParticleManager>();
    //一旦後でRootSignatureについて考える
    particleManager_->Create(pso->GetRootSignature());
    particleManager_->SetCommandListAndSrvDescriptorHeap(commandList, srvDescriptorHeap_.get());
    particleManager_->CreateAll();
    //パーティクルエミッターへのセット
    ParticleEmitter::SetParticleManager(particleManager_.get());

    LogFile::Log("CreateparticleManager");
    //ファイルへのログ出力

#ifdef _DEVELOP
    // デバッグカメラの初期化
    DebugCamera::GetInstance();
    LogFile::Log("Create DebugCamera");
#endif //_DEVELOP

    // =============================================
    // シーンの生成
    // =============================================
    SceneFactory::Create();
    LogFile::Log("CreateScene");

    auto* camera = SceneManager::GetCurrentCamera();
    if (camera) {
        directXCommon_->SetRenderTextureCamera(camera);
        LogFile::Log("Set RenderTexture Camera");
    } else {
        directXCommon_->SetRenderTextureCamera(DebugCamera::GetInstance());
        LogFile::Log("Set Debug Camera");
    }

    LogFile::Log("LoopStart");
}

void ButtobiEngine::Update() {


    if (Window::ProcessMassage()) {
        endRequest_ = true;
    }

    //キーボード情報の取得開始
    input->Update();
#ifdef USE_IMGUI
    //ImGuiにここからフレームが始まる旨を伝える
    imGuiClass.FrameStart();

#endif
    VibrateManager::Update();
    SceneManager::Update();

    auto* camera = SceneManager::GetCurrentCamera();
#ifdef _DEVELOP

    if (camera) {
        directXCommon_->SettingIdTextureBarrierPre();
        //カメラがあるならクリックする
        ObjectManager::GetInstance()->ClickObject(*camera);
        directXCommon_->SettingIdTextureBarrierPost();
    }

    directXCommon_->UpdateGameScreen(srvDescriptorHeap_.get());

#endif //_DEVELOP
    if (camera) {
        // パーティクル管理の更新
        particleManager_->Update(*camera);
    }

    //ポストプロセス用の更新
    directXCommon_->UpdateRenderTexture(srvDescriptorHeap_.get());

}

void ButtobiEngine::Debug()
{
#ifdef USE_IMGUI

    ImGui::Begin("Debug");

    DebugUI::CheckFPS();
    SceneManager::Debug();
    DebugUI::CheckInput();
    DebugUI::CheckLights();
    DebugUI::CheckJsonFile();
    DebugUI::CheckParticle(particleManager_.get());

    ImGui::End();

    //Loaderをここで
    imGuiClass.DrawModelLoaderWindow(srvDescriptorHeap_.get());

#endif // USE_IMGUI

}

ButtobiEngine::~ButtobiEngine()
{
}

void ButtobiEngine::Run() {

    Initialize();

    // =============================================
    // ウィンドウのxボタンが押されるまでループ メインループ
    // =============================================

    while (true) {

        //ループを抜ける
        if (IsEndRequest()) {
            break;
        }

        Update();

        Debug();

        Draw();

    }

    Finalize();

}

void ButtobiEngine::PreCommandSet() {

#ifdef USE_IMGUI
    //ImGuiの内部コマンドを生成する
    imGuiClass.Render();
#endif
    //ポストエフェクトの前設定
    directXCommon_->RenderTexturePreDraw(dsvDescriptorHeap_.get());

    auto* commandList = directXCommon_->GetCommandListClass()->Get();
    //SRV管理の描画前処理
    srvDescriptorHeap_->PreDraw(commandList);

    directXCommon_->VeiwPortAndScissorRect();
#ifdef _DEVELOP
    // デバッグカメラ
    auto* camera = SceneManager::GetCurrentCamera();

    if (camera) {

        DrawGrid::Draw(*camera);
    }

#endif //_DEVELOP

    // シーンの描画
    SceneManager::DrawModel();
    //パーティクルの描画
    particleManager_->Draw();
    //ポストエフェクトのあと設定
    directXCommon_->RenderTexturePostDraw();
    //描画前処理
    directXCommon_->PreDraw();
    //SRV管理の描画前処理
    srvDescriptorHeap_->PreDraw(commandList);
    //ビューポートとシザー
    directXCommon_->VeiwPortAndScissorRect();
}

void ButtobiEngine::PostCommandSet() {

    //ポストエフェクト
    directXCommon_->DrawRenderTexture(rtvDescriptorHeap_.get());
    // シーンの描画
    SceneManager::DrawSprite();

#ifdef USE_IMGUI
    //諸々の描画処理が終了下タイミングでImGuiの描画コマンドを積む
    imGuiClass.DrawImGui(directXCommon_->GetCommandList());

#endif // _DEBUG
    directXCommon_->PostDraw();
    //時間の更新を入れる
    time_->Update();
    //次のコマンド準備をする
    directXCommon_->PrepareCommand();
    //フォントの使用をリセットする
    freeTypeManager_->ResetFontUsage();
};

void ButtobiEngine::Finalize() {

    //シーンマネージャーの終了処理
    SceneManager::Finalize();
    //パーティクルの終了処理
    particleManager_->Finalize();
    particleManager_.reset();

    //モデルの終了処理
    ModelManager::Finalize();

#ifdef _DEVELOP
    //グリットを解放
    DrawGrid::Finalize();
#endif
    //テキストの終了処理
    freeTypeManager_->Finalize();
    freeTypeManager_.reset();
    //テクスチャの終了処理
    texture_->Finalize();
    texture_.reset();

    //音の終了処理
    Sound::Finalize();
    //スプライトの終了処理
    spriteCommon_->Finalize();
    spriteCommon_.reset();

#pragma region//LightManagerの終了処理

    //スポットライトの終了処理
    spotLightManager_->Finalize();
    spotLightManager_.reset();
    //ポイントライトの終了処理
    pointLightManager_->Finalize();
    pointLightManager_.reset();
    //方向ライトの終了処理
    directionalLightManager_->Finalize();
    directionalLightManager_.reset();

#pragma endregion

#ifdef USE_IMGUI
    //ImGuiの終了処理 ゲームループが終わったら行う
    imGuiClass.ShutDown();
#endif
    dsvDescriptorHeap_.reset();
    //RTVManagerのリセット
    rtvDescriptorHeap_.reset();
    //SRVManagerのリセット
    srvDescriptorHeap_.reset();

    //DirecectXCommonのリセット
    directXCommon_->Finalize();
    directXCommon_.reset();


    //バイブレーションの終了処理
    VibrateManager::Finalize();
    //入力クラスの終了処理
    input.reset();
    //ウィンドウクラスの終了処理
    wc->Finalize();
    wc.reset();
    //ログファイルのリセット
    logFile.reset();

}
