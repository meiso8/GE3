#include "Bastet.h"
#include "ParticleEmitter.h"
#include"ModelManager.h"
#include"Model.h"
#include"DebugUI.h"
#include"Lerp.h"
#include"TimeManager.h"
#include"JsonFile.h"
#include"PrimitiveFactory/PrimitiveFactory.h"

namespace {
    const float kAlphaSpeed_ = 0.5f;
    const float kfootStepTime_ = 1.0f;

}
Bastet::Bastet()
{
    obj_ = std::make_unique<Object3d>();
    obj_->Create();
    obj_->SetMeshAndMaterial(ModelManager::GetModel("AmenRa.obj"));
    obj_->SetLightMode(Object3d::kLightModeLReflectance);
    obj_->SetTemperature(0.2f);

    AABB aabb = { .min = {-0.5f,0.0f,-0.5f},.max = {0.5f,1.0f,0.5f} };

    SetCollisionAttribute(0);
    SetCollisionMask(CollisionTag::GetTag("Player") | CollisionTag::GetTag("Enemy")); // プレイヤーや壁と衝突

    //サイズに合わせる
    SetAABB(aabb);
    SetWorldMatrix(obj_->GetWorldTransform().matWorld_);
    //パーティクル作成
    CreateParticle();
}


void Bastet::Initialize() {

    footStepTimer_ = 0.0f;
    step_ = 0;
    isApper_ = false;
    obj_->Initialize();
    obj_->SetObjectName("Bastet");
    obj_->RegisterObject();
    obj_->SetColor({ 1.0f,1.0f,1.0f,0.0f });
    obj_->SetTemperature(0.2f);

#ifdef _DEVELOP
    obj_->SetMeshAndMaterial(PrimitiveFactory::GetPrimitive(Primitive::kPlane));
    obj_->SetScale({ 0.5f,0.5f,0.5f });
    obj_->SetRotate({ 1.57f,0.0f,0.0f });
    obj_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
#endif

}

void Bastet::LoadMap(const std::string& tagName)
{
    //タグ名
    jsonFileName_ = tagName;
#ifdef _DEVELOP
    DebugUI::CreateJsonFile(jsonFileName_.c_str());
#endif

    nlohmann::json& json = JsonFile::GetJsonFiles(jsonFileName_);

    // "transform" キーが存在し、かつそれが配列(Array)であるか安全のためにチェック
    if (json.contains("transform") && json["transform"].is_array()) {

        // 既存のデータをクリア（どんどん追加されてしまうのを防ぐため）
        footSteps_.clear();

        // JSONの配列をループして取り出す
        for (const auto& stepJson : json["transform"]) {
            footSteps_.push_back(JsonFile::JsonToEulerTransform(stepJson));
        }
    }
}

void Bastet::Update() {


    if (!footSteps_.empty()) {

        //最後のステップだったら
        if (step_ == footSteps_.size() - 1) {
            //当たり判定を追加する
            SetCollisionAttribute(CollisionTag::GetTag("Wall"));
        } else {
            SetCollisionAttribute(0);
        }

        if (isApper_) {
            float alpha = Lerp(0.0f, 1.0f, kAlphaSpeed_);
            obj_->SetColor({ 1.0f,1.0f,1.0f,alpha });

        }

        footStepTimer_ -= TimeManager::DeltaTime();
        footStepTimer_ = std::clamp(footStepTimer_, 0.0f, kfootStepTime_);

        if (footStepTimer_ <= 0.0f) {
            //秒数を初期化
            footStepTimer_ = kfootStepTime_;

            //ステップを足す
            step_++;
            step_ %= footSteps_.size();

            //エミッターをゲットする
            auto& emitter0 = particleEmitter_->GetEmitter();
            //更新する
            emitter0.transform.eTransform_ = footSteps_[step_];
            //エミっとする
            particleEmitter_->Emit();
        }


        obj_->SetTranslate(footSteps_[footSteps_.size() - 1].translate);

        obj_->Update();

        particleEmitter_->Update();
    }

#ifdef USE_IMGUI
    // ImGuiの描画処理だけここに書くか、Updateの末尾に移動する


    ImGui::Begin("BastetFoot");

    DebugUI::CheckTransform(obj_->GetTransform(), "ObjStep");

    if (ImGui::Button("AddStep")) {
        footSteps_.push_back(obj_->GetTransform());
    }

    if (ImGui::Button("DeleteStep")) {
        // 配列が空でない場合のみ削除する（空のときに呼ぶとエラーになります）
        if (!footSteps_.empty()) {
            footSteps_.pop_back();
        }
    }

    // 現在登録されているステップの一覧を表示
    for (size_t i = 0; i < footSteps_.size(); ++i) {
        // 各ステップごとに一意のID（"##Delete_0" など）を持つボタンを作成
        std::string buttonLabel = "Delete ##" + std::to_string(i);

        ImGui::Text("Step %d", static_cast<int>(i));
        ImGui::SameLine(); // 横並びにする

        if (ImGui::Button(buttonLabel.c_str())) {
            // 指定したインデックス（i番目）の要素を配列から削除
            footSteps_.erase(footSteps_.begin() + i);

            // 要素を削除するとループのインデックスがズレるため、
            // 1回分のループを終了して抜けるか、ImGuiの描画をリセットします
            break;
        }
    }

    if (ImGui::Button("Save ## FootStep")) {



        nlohmann::json& json = JsonFile::GetJsonFiles(jsonFileName_);

        nlohmann::json footStepsJson = nlohmann::json::array(); // JSONの配列を作成

        for (const auto& step : footSteps_) {
            footStepsJson.push_back(JsonFile::EulerTransformToJson(step));
        }

        json["transform"] = footStepsJson;
        JsonFile::SetJson(jsonFileName_, json);
        // ファイル保存
/*        JsonFile::SaveJson(jsonFileName_);*/
        JsonFile::MarkModified(jsonFileName_);
    }

    ImGui::End();

#endif


}

void Bastet::Draw(Camera& camera) {
    obj_->Draw(camera);

}

void Bastet::OnCollision(Collider* collider) {

    if (isApper_) {
        return;
    }
    if (collider->GetCollisionAttribute() == CollisionTag::GetTag("Player")) {
        // プレイヤーとぶつかったときの処理（必要なら）
        isApper_ = true;
    }

}

void Bastet::CreateParticle()
{

    particleEmitter_ = std::make_unique<ParticleEmitter>();
    particleEmitter_->Initialize();

    auto& emitter0 = particleEmitter_->GetEmitter();
    emitter0.count = 1;
    emitter0.transform.eTransform_.translate = { 0.0f, 0.1f,0.0f };
    emitter0.transform.eTransform_.rotate.x = { Math::kHPi };
    emitter0.startColor = { 1.0f,1.0f,1.0f,0.0f };
    emitter0.endColor = { 1.0f,1.0f,1.0f,0.0f };
    emitter0.frequency = 1.0f;
    emitter0.lifeTime = 3.0f;
    emitter0.blendMode = kBlendModeNormal;
    emitter0.movement = ParticleMovements::kParticleNormal;
    emitter0.useBillboard_ = false;
    emitter0.velocityAABB = { .min = {0.0f,0.0f,0.0f},.max = {0.0f,0.0f,0.0f} };
    emitter0.accelerationField_.acceleration = { 0.0f,0.0f,0.0f };
    particleEmitter_->SetName("footPrints");
}
