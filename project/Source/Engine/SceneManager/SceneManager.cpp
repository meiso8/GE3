#include "SceneManager.h"
#include"DirectXCommon.h"
#include"ObjectManager/ObjectManager.h"
#include"DebugCamera.h"

BaseScene::BaseScene()
{
    sceneChange_ = std::make_unique<SceneChange>();
    sceneChange_->Initialize();
    sceneChange_->SetState(SceneChange::kFadeOut, 1.0f);

    camera_ = std::make_unique<Camera>();
}

void BaseScene::Initialize()
{

}

void BaseScene::Update()
{

}


void BaseScene::SceneChangeUpdate()
{

    sceneChange_->Update();
}

void BaseScene::SwitchCamera()
{
    isDebugCameraActive_ = !isDebugCameraActive_;
    currentCamera_ = (isDebugCameraActive_) ? DebugCamera::GetInstance() : camera_.get();
}

// =========================================================================================

std::map < std::string, std::unique_ptr<BaseScene>> SceneManager::scenes_;
BaseScene* SceneManager::currentScene_ = nullptr;
BaseScene* SceneManager::nextScene_ = nullptr;

void SceneManager::Finalize()
{
    scenes_.clear();
    currentScene_ = nullptr;
    nextScene_ = nullptr;
}

void SceneManager::Update()
{

    currentScene_->SceneChangeUpdate();

    if (currentScene_->GetIsEndScene()) {

        InitScene();
    }

    // シーンの更新処理
    currentScene_->Update();
}

void SceneManager::DrawModel()
{
    currentScene_->DrawModel();
}

void SceneManager::DrawSprite()
{
    currentScene_->DrawSprite();
}


void SceneManager::Debug()
{

#ifdef USE_IMGUI

    ImGui::Begin("Debug");


    std::string currentSceneName = "None";

    for (const auto& [name, scene] : scenes_) {
        if (currentScene_ == scene.get()) {
            currentSceneName = name;
            break;
        }

    }

    if (ImGui::BeginCombo("Change Scene", currentSceneName.c_str())) {

        // マップ内のすべてのシーンをループして選択肢を作る
        for (const auto& [name, scene] : scenes_) {

            // 選択肢を表示（クリックされたら true を返す）
            if (ImGui::Selectable(name.c_str(), true)) {
                // クリックされたらシーン切り替え関数を呼ぶ
                currentScene_->SetStateEnd();
                SetNextScene(name);
                break;
            }
        }

        ImGui::EndCombo();
    }

    if (ImGui::Button("Initialize")) { currentScene_->Initialize(); }

    ImGui::End();

#endif // USE_IMGUI

}

void SceneManager::SetMap(const std::string& name, std::unique_ptr<BaseScene> scene)
{
    scenes_[name] = std::move(scene);
}

void SceneManager::SetNextScene(const std::string& name)
{
    //最初の位置を保持
    nextScene_ = scenes_[name].get();
}

void SceneManager::InitScene()
{
    //次のシーンがセットされていたら次のシーンにする
    if (nextScene_) {
        currentScene_ = nextScene_;
        nextScene_ = nullptr;
    }

    //コマンドを初期化する
    ObjectManager::GetInstance()->Initialize();
    //オブジェクトをクリアする
    ObjectManager::GetInstance()->Clear();
   
    currentScene_->Initialize();

}

