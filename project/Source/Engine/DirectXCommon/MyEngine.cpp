#include "MyEngine.h"
#include"SpriteCommon.h"
#include"Texture.h"
#include"SpriteCamera.h"
#include"DrawGrid.h"
#include"Particle.h"
#include"JsonFile.h"
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

std::unique_ptr <Input> MyEngine::input = nullptr;
std::unique_ptr<Window> MyEngine::wc = nullptr;

DirectXCommon* MyEngine::directXCommon = nullptr;
std::unique_ptr<SrvManager> MyEngine::srvManager = nullptr;
std::unique_ptr<ParticleManager>  MyEngine::particleManager_ = nullptr;
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

    directXCommon =DirectXCommon::GetInstance();
    directXCommon->Initialize(*wc);
    LogFile::Log("CreateDirectXCommon");
    srvManager = std::make_unique<SrvManager>();
    srvManager->Initialize();
    LogFile::Log("CreateSrvManager");
#ifdef USE_IMGUI
    //ImGuiの初期化。
    imGuiClass.Initialize(*wc, directXCommon->GetDevice().Get(), directXCommon->GetSwapChain(), directXCommon->GetSwapChainRtv());
    LogFile::Log("InitImGui");
#endif
    directXCommon->InitializeRenderTexture();
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

    particleManager_ = std::make_unique <ParticleManager>();
    particleManager_->Create();
    particleManager_->CreateAll();
    LogFile::Log("CreateparticleManager");
    //ファイルへのログ出力
    LogFile::Log("LoopStart");


    // =============================================
// シーンの生成
// =============================================

    SceneFactory::Create();

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
    directXCommon->UpdateRenderTexture();
}

void MyEngine::Debug()
{
#ifdef USE_IMGUI
    DebugUI::CheckJsonFile();
    DebugUI::CheckFPS();
    DebugUI::CheckInput();

    DebugUI::CheckSound();
    DebugUI::CheckDirectionalLight();
    DebugUI::CheckSpotLight();
    DebugUI::CheckPointLightData();

    SceneManager::Debug();

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
    // シーンの描画
    SceneManager::DrawModel();
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


    SceneManager::Finalize();

    particleManager_->Finalize();
    particleManager_.reset();

    ModelManager::Finalize();

#ifdef _DEVELOP
    //グリットを解放
    DrawGrid::Finalize();

#endif
    FreeTypeManager::Finalize();

    Texture::Finalize();
    Sound::Finalize();
    SpriteCommon::Finalize();

    SpotLightManager::Finalize();
    PointLightManager::Finalize();
    DirectionalLightManager::Finalize();

#ifdef USE_IMGUI
    //ImGuiの終了処理 ゲームループが終わったら行う
    imGuiClass.ShutDown();
#endif

    directXCommon->EndFrame();

    srvManager.reset();

    VibrateManager::Finalize();
    input.reset();

    wc->Finalize();
    wc.reset();

    logFile.reset();

}
