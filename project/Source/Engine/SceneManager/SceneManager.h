#pragma once
#include"Camera.h"
#include"DebugCamera.h"
#include<memory>
#include<vector>
#include"BaseScene/BaseScene.h"
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

