#include "Beam.h"
#include"DebugUI.h"
#include"MakeMatrix.h"

Beam::Beam()
{
    beam_ = std::make_unique<Primitive>();
    beam_->Create(PrimitiveGenerator::CreateBeam());

    object3d_ = std::make_unique<BeamObject3d>();

    object3d_->Create();
    object3d_->SetMeshAndMaterial(beam_.get());
    object3d_->SetTextureHandle(TextureFactory::BEAM);
}

void Beam::Initialize()
{
    object3d_->Initialize();
}

void Beam::Update()
{

#ifdef _DEVELOP
    DebugUI::CheckObject3d(*object3d_, "Beam");
    ImGui::Begin("BeamPointData");
    auto& point = object3d_->GetPoint();
    ImGui::DragFloat3("PointStartPos", &point.startPos.x);
    ImGui::DragFloat3("PointEndPos", &point.endPos.x);
    ImGui::End();
#endif //_DEVELOP

    if (parent_) {
       Matrix4x4 mat =  object3d_->worldTransform_.matWorld_* *parent_;
       Vector3 pos =  GetWorldTransformByMatrix(mat);
       object3d_->SetStartPos(pos);
    }

    object3d_->Update();

}

void Beam::Draw(Camera* camera)
{
    object3d_->Draw(*camera, BlendMode::kBlendModeAdd, CullMode::kCullModeNone, MaskMode::kZero,true);
}
