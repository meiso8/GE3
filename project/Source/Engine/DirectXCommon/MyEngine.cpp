#include "MyEngine.h"
#include<algorithm>
//#include"TextureManager.h"


const uint32_t MyEngine::kMaxSRVCount = 512;
std::array<PSO, kCountOfBlendMode> MyEngine::pso = {};
MyEngine* MyEngine::instance_ = nullptr;

DirectionalLight* MyEngine::directionalLightData = nullptr;
std::unique_ptr<RootSignature> MyEngine::rootSignature = nullptr;
ModelConfig MyEngine::modelConfig_ = {};

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

    logFile = std::make_unique<LogFile>();
    logFile->Create();
    LogFile::Log("CreateLogFile");

    //WindowClassの生成
    wc = std::make_unique<Window>();
    wc->Create(title, clientWidth, clientHeight);
    LogFile::Log("CreateWindowClass");

    //InputClassの生成
    input = Input::GetInstance();
    //入力
    input->Initialize(*wc);

    directXCommon = std::make_unique<DirectXCommon>();
    directXCommon.get()->Initialize(*wc);

    //TextureManagerの初期化

#pragma region//RootSignatureを生成する

    rootSignature = std::make_unique<RootSignature>();
    //具体的にShaderがどこかでデータを読めばいいのかの情報を取りまとめたもの
    rootSignature->Create();
    LogFile::Log("CreateRootSignature");

#pragma endregion

#pragma region//InputLayout
    inputLayout = std::make_unique<InputLayout>();
    inputLayout->Create();
    LogFile::Log("InputLayout");
#pragma endregion

    //BlendStateの設定を行う

    blendStates.resize(kCountOfBlendMode);
    for (int i = 0; i < blendStates.size(); ++i) {
        blendStates[i].Create(i);
    }

    LogFile::Log("SetBlendState");


    rasterizerStates.resize(3);
    //RasterizerStateの設定を行う
    rasterizerStates[kCullModeNone].Create(kCullModeNone, kFillModeSolid);//ソリッドモードカリングなし
    rasterizerStates[kCullModeFront].Create(kCullModeFront, kFillModeSolid);//ソリッドモード裏面
    rasterizerStates[kCullModeBack].Create(kCullModeBack, kFillModeSolid);//ソリッドモード表面
    //rasterizerStates[0].Create(kCullModeNone, kFillModeWireframe);//ワイヤーフレームモード
    LogFile::Log("SetRasterizerState");

#pragma region //DepthStencilStateの設定
    depthStencil.Create();
    LogFile::Log("Create depthStencilDesc");
#pragma endregion

    //PSOを生成する
    pso[0].Create(
        *rootSignature,
        *inputLayout,
        blendStates[kBlendModeNone],//ブレンドしない
        rasterizerStates[kCullModeBack],//後ろをカリング
        depthStencil);

    pso[1].Create(
        *rootSignature,
        *inputLayout,
        blendStates[kBlendModeNormal],//ブレンドする
        rasterizerStates[kCullModeBack],//後ろをカリング
        depthStencil);

    pso[2].Create(
        *rootSignature,
        *inputLayout,
        blendStates[kBlendModeAdd],//ブレンドしない
        rasterizerStates[kCullModeBack],//描画
        depthStencil);

    pso[3].Create(
        *rootSignature,
        *inputLayout,
        blendStates[kBlendModeSubtract],
        rasterizerStates[kCullModeBack],//描画
        depthStencil);

    pso[4].Create(
        *rootSignature,
        *inputLayout,
        blendStates[kBlendModeMultiply],
        rasterizerStates[kCullModeBack],//描画
        depthStencil);

    pso[5].Create(
        *rootSignature,
        *inputLayout,
        blendStates[kBlendModeScreen],
        rasterizerStates[kCullModeBack],//描画
        depthStencil);

    LogFile::Log("CreatePSO");

#pragma region//平行光源用のResourceを作成する
    directionalLightResource = DirectXCommon::CreateBufferResource(sizeof(DirectionalLight));
    //書き込むためのアドレスを取得
    directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));
    //デフォルト値はとりあえず以下のようにしておく   
    directionalLightData->color = { 1.0f,1.0f,1.0f,1.0f };
    directionalLightData->direction = { 0.0f,1.0f,0.0f };//向きは正規化する
    directionalLightData->intensity = 1.0f;

#pragma endregion


    modelConfig_.Initialize(rootSignature.get(), directionalLightResource.Get());

    srand(static_cast<unsigned int>(time(nullptr)));

    //ファイルへのログ出力
    LogFile::Log("LoopStart");

}

void MyEngine::Update() {

    //キーボード情報の取得開始
    input->Update();
    directXCommon.get()->Update();

}

void MyEngine::PreCommandSet(Vector4& color) {
    directXCommon.get()->PreDraw(color);
};

void MyEngine::PostCommandSet() {

    directXCommon.get()->PostDraw();

};

void MyEngine::End() {

    directXCommon.get()->EndFrame();
    wc->Finalize();
    
    //TextureManager::GetInstance()->Finalize();
}


//ここでBlenModeを変更する
void MyEngine::SetBlendMode(uint32_t blendMode) {

    directXCommon.get()->GetCommandList()->SetPipelineState(pso[blendMode].GetGraphicsPipelineState(PSO::PSOType::TRIANGLE).Get());//PSOを設定

}