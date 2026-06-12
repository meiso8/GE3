#include "Beam.h"
#include"DebugUI.h"
#include"MakeMatrix.h"
#include"TimeManager.h"
#include"Easing.h"
#include"Sound.h"

namespace {
    const float kBeamDuretion_ = 1.5f;
    const float kLigeDuration_ = 5.0f;
    
}

Beam::Beam()
{
    beam_ = std::make_unique<Primitive>();
    beam_->Create(PrimitiveGenerator::CreateBeam(1.0f));

    object3d_ = std::make_unique<Object3d>();

    object3d_->Create();
    object3d_->SetMeshAndMaterial(beam_.get());
    object3d_->SetTextureHandle(TextureFactory::BEAM);
    object3d_->SetTemperature(1.0f);

#ifdef _DEBUG
    line_ = std::make_unique<LineMesh>();
    std::unique_ptr<MeshData> meshData = std::make_unique<MeshData>();
    *meshData = PrimitiveGenerator::CreateLine({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f });
    line_->Create(std::move(meshData));

    lineObj_ = std::make_unique<Object3d>();
    lineObj_->Create();
    lineObj_->SetColor({ 1.0f,0.0f,0.0f,1.0f });
    lineObj_->SetMeshAndMaterial(line_.get());
#endif



    type_ = kEnemy;
}

void Beam::Initialize()
{
    object3d_->Initialize();
    isActive_ = false;
    ray_ = { .origin = {0.0f,0.0f,0.0f},.diff = {0.0f,0.0f,0.0f} };
    type_ = kEnemy;
    //親をなくす
    parent_ = nullptr;
    point_ = { 0.0f };
    
    //場外移動する
    object3d_->Update();
    object3d_->SetLightMode(kLightModeNone);


#ifdef _DEBUG
    line_->SetVertex(ray_.origin, ray_.diff + ray_.origin);

#endif



}


Vector3 CalculateLookAtRotate(const Vector3& startPos, const Vector3& endPos) {
    // 進行方向のベクトルを計算
    Vector3 diff = endPos - startPos;

    // XZ平面上（地面）での移動距離を計算（三平方の定理）
    float horizontalLength = std::sqrt(diff.x * diff.x + diff.z * diff.z);

    Vector3 rotate = { 0.0f, 0.0f, 0.0f };

    // 1. Y軸まわりの回転 (Yaw: 左右の向き)
    // Z前方を基準として、Xにどれだけ傾いているか
    rotate.y = std::atan2(diff.x, diff.z);

    // 2. X軸まわりの回転 (Pitch: 上下の傾き)
    // 水平に進む距離に対して、Y（高さ）がどれだけ変化しているか
    // ※DirectXの座標系に合わせて、下を向く（Yマイナス）のときにプラス回転になるよう符号を調整しています
    rotate.x = std::atan2(-diff.y, horizontalLength);

    // 3. Z軸まわりの回転 (Roll: ひねり)
    // レーザーの軸回転は不要なので 0 固定
    rotate.z = 0.0f;

    return rotate;
}

void Beam::Update()
{

#ifdef USE_IMGUI

    DebugUI::CheckObject3d(*object3d_, "Beam");

    ImGui::Begin("BeamPointData");

    if (ImGui::TreeNode("Beams")) {

        ImGui::Text("type", "%s", type_ == kPlayer ? "player" : "enemy");

        ImGui::DragFloat3("Point StartPos", &point_.startPos.x);
        ImGui::DragFloat3("Point EndPos", &point_.endPos.x);

        ImGui::DragFloat3("StartPos", &startPos_.x);
        ImGui::DragFloat3("tEndPos", &endPos_.x);

        ImGui::DragFloat3("ray_origin", &ray_.origin.x);
        ImGui::DragFloat3("ray_diff", &ray_.diff.x);

        ImGui::Checkbox("isActive", &isActive_);
        ImGui::DragFloat("lifeTimer", &lifeTimer_);

        ImGui::TreePop();

    }

    ImGui::End();

#endif //USE_IMGUI

    if (!isActive_) {
        return;
    }

    if (lifeTimer_ <= 0.0f) {
        isActive_ = false;
        //一応生存時間を0にしておく
        lifeTimer_ = 0.0f;
        Initialize();
        return;
    } else {
        lifeTimer_ -= Time::DeltaTime();
    }

    float time =  (kLigeDuration_ - lifeTimer_) / kBeamDuretion_;
    time = std::clamp(time, 0.0f, 1.0f);

    Vector3 pos = { 0.0f };

    if (parent_) {
        pos = GetWorldTransformByMatrix(*parent_);
    } else {
        pos = startPos_;
    }

    point_ = { .startPos = pos ,.endPos = Lerp(pos, endPos_, time) };

    UpdateObject();

}

void Beam::Draw(Camera* camera)
{

#ifdef _DEBUG
    lineObj_->Draw(*camera);
#endif

    if (!isActive_) {
        return;
    }

    object3d_->SetLightMode(kLightModeNone);
    object3d_->Draw(*camera, BlendMode::kBlendModeAdd, CullMode::kCullModeNone, MaskMode::kZero, true);


}


bool Beam::Shot(const Vector3& target, const  BeamType& type, const Vector3& startPos, Matrix4x4* parent)
{

    endPos_ = target;
    type_ = type;

    parent_ = parent;

    Vector3 pos = { 0.0f };

    if (parent_) {
        pos = GetWorldTransformByMatrix(*parent_);
    } else {
        pos = startPos_;
    }

    point_ = { .startPos = pos,.endPos = pos };


    UpdateObject();

    lifeTimer_ = kLigeDuration_;
    isActive_ = true;

    return true;
}

void Beam::UpdateObject()
{

    ray_ = { .origin = point_.startPos,.diff = point_.endPos - point_.startPos };

#ifdef _DEBUG
    line_->SetVertex(point_.startPos, point_.endPos);
    lineObj_->Update();

#endif


    Vector3 rotate = CalculateLookAtRotate(point_.startPos, point_.endPos);
    float scaleZ = Length(point_.endPos - point_.startPos);
    Vector3 centerPos =  point_.endPos;

    object3d_->worldTransform_.rotate_ = rotate;
    object3d_->worldTransform_.scale_ = { 1.0f,1.0f,scaleZ };
    object3d_->worldTransform_.translate_ = centerPos;

    object3d_->Update();
}
