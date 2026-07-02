#pragma once

#include<memory>
#include<vector>
#include"Camera.h"
#include"DebugCamera.h"
#include"SceneChange.h"

class BaseScene
{
protected:
    //カメラ
    bool isDebugCameraActive_ = false;

    std::unique_ptr<Camera> camera_ = nullptr;

    Camera* currentCamera_ = nullptr;
    std::unique_ptr <SceneChange> sceneChange_ = nullptr;

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
