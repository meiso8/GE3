#include "Beam.h"
#include"DebugUI.h"

Beam::Beam()
{
    beam_ = std::make_unique<Primitive>();
    beam_->Create(PrimitiveGenerator::CreateBeam());

    object3d_ = std::make_unique<Object3d>();

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
#endif //_DEVELOP

    object3d_->Update();

}

void Beam::Draw(Camera* camera)
{
    object3d_->Draw(*camera, BlendMode::kBlendModeAdd, CullMode::kCullModeNone, MaskMode::kZero,true);
}
