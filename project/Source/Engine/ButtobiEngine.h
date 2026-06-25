#pragma once
#include<memory>
#include"D3DResourceLeakChecker.h"
#include<string>
#include<stdint.h>
#include"ImGuiClass.h"

#include"TimeManager.h"
#include"SRVmanager/SrvManager.h"
#include"Rtvmanager/Rtvmanager.h"
#include"Input.h"
#include"Log.h"
#include"PrimitiveFactory/PrimitiveFactory.h"
#include"Engine/FreeTypeManager/FreeTypeManager.h"
#include"Particle.h"
#include"Lights/PointLightManager.h"
#include"Lights/DirectionalLightManager.h"
#include"Lights/SpotLightManager.h"
#include"Texture.h"

#include"SpriteCommon.h"

class DirectXCommon;

class ButtobiEngine {
private:
    /// @brief リークチェッカー
    D3DResourceLeakChecker leakChecker;
    /// @brief エンドリクエストフラグ
    bool endRequest_ = false;
    /// @brief ログファイル
    std::unique_ptr<LogFile> logFile = nullptr;
    /// @brief ウィンドウクラス
    std::unique_ptr<Window> wc = nullptr;
    /// @brief 入力
    std::unique_ptr <Input> input = nullptr;
    /// @brief 時間管理
    std::unique_ptr<TimeManager> time_ = nullptr;
    /// @brief DirectXCommon後で改変する
    std::unique_ptr<DirectXCommon> directXCommon_ = nullptr;
    /// @brief RTV管理
    std::unique_ptr<RtvManager> rtvManager = nullptr;
    /// @brief SRV管理
    std::unique_ptr<SrvManager> srvManager = nullptr;

    /// @brief 方向ライト
    std::unique_ptr<DirectionalLightManager> directionalLightManager_ = nullptr;
    /// @brief ポイントライト
    std::unique_ptr<PointLightManager> pointLightManager_ = nullptr;
    /// @brief スポットライト
    std::unique_ptr<SpotLightManager> spotLightManager_ = nullptr;

#ifdef USE_IMGUI
    ImGuiClass imGuiClass = {};
#endif // USE_IMGUI

    /// @brief　プリミティブ
    std::unique_ptr<PrimitiveFactory> primitiveFactory_ = nullptr;
   
    /// @brief　パーティクル管理
    std::unique_ptr<ParticleManager> particleManager_ = nullptr;
    /// @brief テクスチャ
    std::unique_ptr<Texture> texture_ = nullptr;
    /// @brief スプライト
    std::unique_ptr<SpriteCommon> spriteCommon_ = nullptr;
    /// @brief テキスト管理
    std::unique_ptr<FreeTypeManager> freeTypeManager_ = nullptr;
private:
    void SetCommandList(ID3D12GraphicsCommandList* commandList);

protected:
    
    //継承するもの

    void Create(const std::wstring& title, const int32_t clientWidth, const int32_t clientHeight);
    void PreCommandSet();
    void PostCommandSet();
    /// @brief 初期化処理　純粋仮想関数
    virtual void Initialize() = 0;
    virtual void Draw() = 0;
    virtual void Finalize();
    virtual void Update();
    virtual void Debug();
    bool IsEndRequest() { return endRequest_; }

public:
    ButtobiEngine() = default;
    ButtobiEngine(ButtobiEngine&) = delete;
    ButtobiEngine& operator=(ButtobiEngine&) = delete;
    ~ButtobiEngine();
    void Run();
};

