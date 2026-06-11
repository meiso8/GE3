#include "Beam.h"
#include"DebugUI.h"
#include"MakeMatrix.h"
#include"TimeManager.h"
#include"Easing.h"
#include"Sound.h"

namespace {
    const float kBeamDuretion_ = 0.5f;
    const float kLigeDuration_ = 5.0f;

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
}

void Beam::Initialize()
{
    object3d_->Initialize();
    isActive_ = false;
}

void Beam::Update()
{

#ifdef USE_IMGUI
    DebugUI::CheckObject3d(*object3d_, "Beam");
    ImGui::Begin("BeamPointData");
    auto& point = object3d_->GetPoint();
    ImGui::DragFloat3("PointStartPos", &point.startPos.x);
    ImGui::DragFloat3("PointEndPos", &point.endPos.x);
    ImGui::Checkbox("isActive", &isActive_);
    ImGui::DragFloat("lifeTimer", &lifeTimer_);
    ImGui::End();
#endif //USE_IMGUI

    if (!isActive_) {
        return;
    }

    if (lifeTimer_ <= 0.0f) {
        isActive_ = false;
        //一応生存時間を0にしておく
        lifeTimer_ = 0.0f;
        return;
    } else {
        lifeTimer_ -= Time::DeltaTime();
    }

    if (parent_) {
        float time = 1.0f-lifeTimer_ / kBeamDuretion_;
        time = std::clamp(time, 0.0f, 1.0f);

        Matrix4x4 mat = *parent_;
        Vector3 pos = GetWorldTransformByMatrix(mat);
        
        object3d_->SetStartPos(pos);
        object3d_->SetEndPos(Lerp(pos, endPos_,time));
    }
    


    object3d_->Update();

}

void Beam::Draw(Camera* camera)
{
    if (!isActive_) {
        return;
    }

    object3d_->Draw(*camera, BlendMode::kBlendModeAdd, CullMode::kCullModeNone, MaskMode::kAll, true);
}

bool Beam::Shot(const Vector3& target)
{
    Sound::PlaySE(SoundFactory::CRACKER);
    endPos_ = target;
 
    if (parent_) {
        Matrix4x4 mat = *parent_;
        Vector3 pos = GetWorldTransformByMatrix(mat);
        object3d_->SetStartPos(pos);
        object3d_->SetEndPos(pos);
    }

    object3d_->Update();
    lifeTimer_ = kLigeDuration_;
    isActive_ = true;

    return true;
}
