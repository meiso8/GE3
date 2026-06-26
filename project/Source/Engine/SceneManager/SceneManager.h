#pragma once
#include"Camera.h"
#include"DebugCamera.h"
#include<memory>
#include<vector>
#include"SceneChange.h"
#include"LevelEditor/LevelEditor.h"

class BaseScene
{
protected:
    //カメラ
    bool isDebugCameraActive_ = false;

    std::unique_ptr<Camera> camera_ = nullptr;

    Camera* currentCamera_ = nullptr;
    std::unique_ptr <SceneChange> sceneChange_ = nullptr;
    //レベルエディタ
    std::unique_ptr<LevelEditor> levelEditor_ = nullptr;
    std::vector<std::unique_ptr<LevelEditor::ObjectSet>>objects_;

public:
    Camera* GetCurrentCamera() { return currentCamera_; };
    BaseScene();
    virtual ~BaseScene() = default;
    virtual void Initialize();
    virtual void Update();
    virtual void DrawModel() = 0;
    virtual void DrawSprite() = 0;
    virtual void SceneChangeUpdate();
    bool GetIsEndScene() {
        if (sceneChange_ == nullptr) {
            return false;
        }
        // nullptr でない場合は IsEndScene() を呼び出す
        return sceneChange_->IsEndScene();
    };

    void SetStateEnd() { sceneChange_->SetState(SceneChange::kSceneEnd, 0.0f); }

    void SwitchCamera();
};

#include<map>

class SceneManager {
public:
    static void Finalize();
    static void Update();
    static void DrawModel();
    static void DrawSprite();
    static void Debug();
    static void SetMap(const std::string& name, std::unique_ptr<BaseScene> scene);
    static void SetNextScene(const std::string& name);
    static void InitScene();
    static Camera* GetCurrentCamera() { return currentScene_->GetCurrentCamera(); };
    static std::string GetCurrentSceneName() { if (currentScene_) return sceneNames_.at(currentScene_); return "noScene"; }
private:
    static BaseScene* currentScene_;
    static BaseScene* nextScene_;
    static std::map < std::string, std::unique_ptr<BaseScene>> scenes_;
    static std::map <  BaseScene*, std::string> sceneNames_;
};

