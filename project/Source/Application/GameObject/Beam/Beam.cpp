#include "Beam.h"
#include"DebugUI.h"
#include"MakeMatrix.h"
#include"TimeManager.h"
#include"Easing.h"
#include"Sound.h"

namespace {
    const float kBeamDuretion_ = 0.5f;
    const float kLigeDuration_ = 10.0f;

}

Beam::Beam()
{
    beam_ = std::make_unique<Primitive>();
    beam_->Create(PrimitiveGenerator::CreateBeam(1.0f));
     
    object3d_ = std::make_unique<BeamObject3d>();

    object3d_->Create();
    object3d_->SetMeshAndMaterial(beam_.get());
    object3d_->SetTextureHandle(TextureFactory::BEAM);
    object3d_->SetTemperature(1.0f);


    line_ = std::make_unique<LineMesh>();
    std::unique_ptr<MeshData> meshData = std::make_unique<MeshData>();
    *meshData = PrimitiveGenerator::CreateLine({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f });
    line_->Create(std::move(meshData));

    lineObj_ = std::make_unique<Object3d>();
    lineObj_->Create();
    lineObj_->SetColor({ 1.0f,0.0f,0.0f,1.0f });
    lineObj_->SetMeshAndMaterial(line_.get());

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

    //場外移動する
    object3d_->SetStartPos({0.0f,-10.0f,0.0f});
    object3d_->SetEndPos({0.0f,-10.0f,0.0f});
    object3d_->Update();
    object3d_->SetLightMode(kLightModeNone);


    line_->SetVertex(ray_.origin, ray_.diff + ray_.origin);
}

void Beam::Update()
{

#ifdef USE_IMGUI

    DebugUI::CheckObject3d(*object3d_, "Beam");

    ImGui::Begin("BeamPointData");

    if (ImGui::TreeNode("Beams")) {

        auto& point = object3d_->GetPoint();
        ImGui::Text("type", "%s", type_ == kPlayer ? "player" : "enemy");
        ImGui::DragFloat3("PointStartPos", &point.startPos.x);
        ImGui::DragFloat3("PointEndPos", &point.endPos.x);

        ImGui::DragFloat3("ray_origin", &ray_.origin.x);
        ImGui::DragFloat3("ray_diff", &ray_.diff.x);

        ImGui::Checkbox("isActive", &isActive_);
        ImGui::DragFloat("lifeTimer", &lifeTimer_);

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

    float time = 1.0f -(std::min( lifeTimer_, kBeamDuretion_) / kBeamDuretion_);
    time = std::clamp(time, 0.0f, 1.0f);

    Vector3 pos = { 0.0f };

    if (parent_) {
        pos = GetWorldTransformByMatrix(*parent_);   
    } else {
        pos = startPos_;
    }

    object3d_->SetStartPos(pos);

    object3d_->SetEndPos(Lerp(pos, endPos_, time));

    auto& point = object3d_->GetPoint();
    ray_ = { .origin = point.startPos,.diff = point.endPos +point.startPos};

    object3d_->Update();


    line_->SetVertex(point.startPos, point.endPos);


    lineObj_->Update();
 
}

void Beam::Draw(Camera* camera)
{


    lineObj_->Draw(*camera);


    if (!isActive_) {
        return;
    }

    object3d_->SetLightMode(kLightModeNone);
    object3d_->Draw(*camera, BlendMode::kBlendModeAdd, CullMode::kCullModeNone, MaskMode::kZero, true);


}


bool Beam::Shot(const Vector3& target,const  BeamType& type,const Vector3& startPos,Matrix4x4* parent)
{
    Sound::PlaySE(SoundFactory::CRACKER);
    endPos_ = target;
    type_ = type;
    
    parent_ = parent;

    Vector3 pos = { 0.0f };

    if (parent_) {
         pos = GetWorldTransformByMatrix(*parent_);
    } else {
        pos = startPos_;
    }

    object3d_->SetStartPos(pos);
    object3d_->SetEndPos(pos);

    auto& point = object3d_->GetPoint();
    ray_ = { .origin = point.startPos,.diff = { 0.0f} };
    
    line_->SetVertex(point.startPos, point.endPos);

    object3d_->Update();

    lifeTimer_ = kLigeDuration_;
    isActive_ = true;

    return true;
}
