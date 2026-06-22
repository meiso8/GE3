#pragma once

#pragma region //自作関数
#include"D3DResourceLeakChecker.h"

#include"Input.h"
#include"DirectXCommon.h"
#include"SRVmanager/SrvManager.h"
#include"Log.h"
#include<memory>

#pragma endregion

class MyEngine {
private:
    
    D3DResourceLeakChecker leakChecker;

    static DirectXCommon* directXCommon;
#ifdef USE_IMGUI
    ImGuiClass imGuiClass = {};
#endif // USE_IMGUI

    static std::unique_ptr<LogFile> logFile;
    static std::unique_ptr<Window> wc;
    static std::unique_ptr <Input> input;
    static std::unique_ptr<SrvManager> srvManager;
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
    MyEngine() = default;
    MyEngine(MyEngine&) = delete;
    MyEngine& operator=(MyEngine&) = delete;
    virtual ~MyEngine() = default;
    void Run();

};

