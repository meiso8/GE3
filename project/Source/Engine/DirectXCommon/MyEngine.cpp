#include "MyEngine.h"
#include"SpriteCommon.h"
#include"Texture.h"
#include"SpriteCamera.h"
#include"ModelManager.h"
#include"DrawGrid.h"
#include"Particle.h"
#include"JsonFile.h"
#include"Sound.h"
#include"VibrateManager.h"
#include"Lights/PointLightManager.h"
#include"Lights/DirectionalLightManager.h"
#include"Lights/SpotLightManager.h"
#include"SceneManager.h"
#include"SceneFactory.h"
#include"Application/Loader/ModelFactory.h"
#include"Application/Loader/SoundFactory.h"
#include"Application/Loader/TextureFactory.h"
#include"Engine/FreeTypeManager/FreeTypeManager.h"
#include"ObjectManager/ObjectManager.h"

#include"PSO.h"
#include"CrashHandler.h"

#include"DebugUI.h"
#include"DebugCamera.h"

std::unique_ptr <Input> MyEngine::input = nullptr;
std::unique_ptr<Window> MyEngine::wc = nullptr;

DirectXCommon* MyEngine::directXCommon = nullptr;
std::unique_ptr<SrvManager> MyEngine::srvManager = nullptr;
std::unique_ptr<RtvManager>MyEngine::rtvManager = nullptr;
std::unique_ptr<LogFile> MyEngine::logFile = nullptr;

void MyEngine::Create(const std::wstring& title, const int32_t clientWidth, const int32_t clientHeight) {


    //誰も捕捉しなかった場合に(Unhandled),補足する関数を登録
    //main関数始まってすぐに登録すると良い
    SetUnhandledExceptionFilter(ExportDump);

    logFile = std::make_unique<LogFile>();
    logFile->Create();
    LogFile::Log("CreateLogFile");

    //WindowClassの生成
    wc = std::make_unique<Window>();
    wc->Create(title, clientWidth, clientHeight);
    LogFile::Log("CreateWindowClass");

    //InputClassの生成
    input = std::make_unique<Input>();
    //入力
    input->Initialize(*wc);
    LogFile::Log("CreateInput");
    //コントローラー
    VibrateManager::Initialize();

    directXCommon = DirectXCommon::GetInstance();
    directXCommon->PreInitialize(*wc);
    LogFile::Log("DirectXCommon PreInitialize");

    rtvManager = std::make_unique<RtvManager>();
    rtvManager->Initialize();
    LogFile::Log("CreateRtvManager");

    directXCommon->InitializeRenderTargetView(*rtvManager);
    LogFile::Log("DirectXCommon InitializeRenderTargetView");

    directXCommon->PostInitialize();
    LogFile::Log("DirectXCommon PostInitialize");

    srvManager = std::make_unique<SrvManager>();
    srvManager->Initialize();
    LogFile::Log("CreateSrvManager");

#ifdef USE_IMGUI
    //ImGuiの初期化。
    imGuiClass.Initialize(*wc, directXCommon->GetDevice().Get(), directXCommon->GetSwapChain(), directXCommon->GetSwapChainRtv());
    LogFile::Log("InitImGui");
#endif
    
    directXCommon->InitializeRenderTexture(*rtvManager);
    directXCommon->CreateDepthStencilResourceSRV();

    auto* pso = PSO::GetInstance();
    pso->CreateALLPSO();

    LogFile::Log("CreatePSO");

    DirectionalLightManager::Create();
    LogFile::Log("CreateDirectionalLightResource");
    PointLightManager::CreateData();
    SpotLightManager::Create();
    ////共通のスプライト
    SpriteCommon::Initialize();
    LogFile::Log("InitializeSpriteCommon");

    //スプライト用カメラ
    SpriteCamera::Initialize(static_cast<float>(wc->GetClientWidth()), static_cast<float>(wc->GetClientHeight()));
    LogFile::Log("InitializeSpriteCamera");
    //サウンド管理
    Sound::Initialize();
    LogFile::Log("InitializeSound");
    //テクスチャ管理
    Texture::Initialize();
    LogFile::Log("InitializeTexture");
    //テスクチャ読み込み
    TextureFactory::Load();
    LogFile::Log("LoadAllTexture");
    //音声の読み込み
    SoundFactory::Load();
    LogFile::Log("LoadAllSound");
    //モデル読み込み
    ModelFactory::Load();
    LogFile::Log("LoadAllModel");
    //JsonFileの読み込み
    JsonFile::LoadAllJsonFile();
    LogFile::Log("LoadAllJsonFile");
    FreeTypeManager::Initialize();
    LogFile::Log("InitializeFreeTypeManager");
#ifdef _DEVELOP
    //グリット描画
    DrawGrid::Create();
    LogFile::Log("CreateDrawGrid");
#endif

    auto* particleManager = ParticleManager::GetInstance();
    particleManager->Create();
    particleManager->CreateAll();

    LogFile::Log("CreateparticleManager");
    //ファイルへのログ出力

#ifdef _DEVELOP
    // デバッグカメラの初期化
    DebugCamera::GetInstance();
    LogFile::Log("Create DebugCamera");
#endif //_DEVELOP

    //オブジェクト管理の初期化
    ObjectManager::GetInstance()->Initialize();
    LogFile::Log("ObjectManager Initialize");

    // =============================================
    // シーンの生成
    // =============================================
    SceneFactory::Create();
    LogFile::Log("CreateScene");

    auto* camera = SceneManager::GetCurrentCamera();
    if (camera) {
        DirectXCommon::GetInstance()->SetRenderTextureCamera(camera);
        LogFile::Log("Set RenderTexture Camera");
    }

    LogFile::Log("LoopStart");
}

void MyEngine::Update() {


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

#ifdef _DEVELOP
    auto* camera = SceneManager::GetCurrentCamera();
    if (camera) {
        directXCommon->SettingIdTextureBarrierPre();
        LogFile::Log("SettingIdTextureBarrierPre");
        //カメラがあるならクリックする
        ObjectManager::GetInstance()->ClickObject(*camera);
        LogFile::Log("ObjectManager ClickObject");
        directXCommon->SettingIdTextureBarrierPost();
        LogFile::Log("SettingIdTextureBarrierPost");
        // 共通更新
        ParticleManager::GetInstance()->Update(*camera);
        LogFile::Log("ParticleManager Update");
    }

#endif //_DEVELOP

    directXCommon->UpdateRenderTexture();

}

void MyEngine::Debug()
{
#ifdef USE_IMGUI

    ImGui::Begin("Debug");

    DebugUI::CheckFPS();

    auto* camera = SceneManager::GetCurrentCamera();

    if (camera) {
        DebugUI::CheckCamera(*camera);
    }

    SceneManager::Debug();
    DebugUI::CheckInput();
    DebugUI::CheckLights();
    DebugUI::CheckJsonFile();
    DebugUI::CheckParticle();

    ImGui::End();

    //Loaderをここで
    imGuiClass.DrawModelLoaderWindow();

#endif // USE_IMGUI

}

void MyEngine::Run() {

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

void MyEngine::PreCommandSet() {

#ifdef USE_IMGUI
    //ImGuiの内部コマンドを生成する
    imGuiClass.Render();
#endif
    //ポストエフェクトの前設定
    directXCommon->RenderTexturePreDraw();

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
    ParticleManager::GetInstance()->Draw();

    //ポストエフェクトとのあと設定
    directXCommon->RenderTexturePostDraw();

    directXCommon->PreDraw();
}

void MyEngine::PostCommandSet() {

    //ポストエフェクト
    directXCommon->DrawRenderTexture();
    // シーンの描画
    SceneManager::DrawSprite();

#ifdef USE_IMGUI
    //諸々の描画処理が終了下タイミングでImGuiの描画コマンドを積む
    imGuiClass.DrawImGui(CommandList::GetCommandList().Get());

#endif // _DEBUG
    directXCommon->PostDraw();

    FreeTypeManager::ResetFontUsage();
};

void MyEngine::Finalize() {

    //シーンマネージャーの終了処理
    SceneManager::Finalize();
    //パーティクルの終了処理
    ParticleManager::GetInstance()->Finalize();
    //モデルの終了処理
    ModelManager::Finalize();

#ifdef _DEVELOP
    //グリットを解放
    DrawGrid::Finalize();
#endif
    //テキストの終了処理
    FreeTypeManager::Finalize();
    //テクスチャの終了処理
    Texture::Finalize();
    //音の終了処理
    Sound::Finalize();
    //スプライトの終了処理
    SpriteCommon::Finalize();
    //スポットライトの終了処理
    SpotLightManager::Finalize();
    //ポイントライトの終了処理
    PointLightManager::Finalize();
    //方向ライトの終了処理
    DirectionalLightManager::Finalize();

#ifdef USE_IMGUI
    //ImGuiの終了処理 ゲームループが終わったら行う
    imGuiClass.ShutDown();
#endif
    //DirectXCommonの終了処理
    directXCommon->EndFrame();
    //RTVManagerのリセット
    rtvManager.reset();
    //SRVManagerのリセット
    srvManager.reset();
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
