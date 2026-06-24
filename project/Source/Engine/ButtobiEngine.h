#pragma once
#include<memory>
#include"D3DResourceLeakChecker.h"
#include<string>
#include<stdint.h>
#include"ImGuiClass.h"
#include"DirectXCommon.h"
#include"TimeManager.h"
#include"SRVmanager/SrvManager.h"
#include"Rtvmanager/Rtvmanager.h"
#include"Input.h"
#include"Log.h"
#include"PrimitiveFactory/PrimitiveFactory.h"

class ButtobiEngine {
private:
    D3DResourceLeakChecker leakChecker;
    static DirectXCommon* directXCommon;
#ifdef USE_IMGUI
    ImGuiClass imGuiClass = {};
#endif // USE_IMGUI

    std::unique_ptr<LogFile> logFile = nullptr;
    std::unique_ptr<Window> wc = nullptr;
    std::unique_ptr <Input> input = nullptr;
    std::unique_ptr<SrvManager> srvManager = nullptr;
    std::unique_ptr<RtvManager> rtvManager = nullptr;
    std::unique_ptr<TimeManager> time_ = nullptr;
    std::unique_ptr<PrimitiveFactory> primitiveFactory_ = nullptr;
    bool endRequest_ = false;
protected:

    void Create(const std::wstring& title, const int32_t clientWidth, const int32_t clientHeight);
    void PreCommandSet();
    void PostCommandSet();

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
    virtual ~ButtobiEngine() = default;
    void Run();
};

