#include "SceneManager.h"
#include"DirectXCommon.h"
#include"ObjectManager/ObjectManager.h"

std::map < std::string, std::unique_ptr<BaseScene>> SceneManager::scenes_;
std::map <BaseScene*, std::string> SceneManager::sceneNames_;
BaseScene* SceneManager::currentScene_ = nullptr;
BaseScene* SceneManager::nextScene_ = nullptr;

void SceneManager::Finalize()
{
    scenes_.clear();
    sceneNames_.clear();
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

    if (ImGui::BeginCombo("Change Scene", GetCurrentSceneName().c_str())) {

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
    sceneNames_[scenes_[name].get()] = name;
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

