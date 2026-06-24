#include "Field.h"
#include"Input.h"
#include"DebugUI.h"
#include"TimeManager.h"
Field::Field()
{

    circle_ = { {0.0f,0.0f,0.0f},25.0f };
    circleMesh_ = std::make_unique<Primitive>();
    circleMesh_->Create(PrimitiveGenerator::CreateCircle(circle_));

    object3d_ = std::make_unique<Object3d>();
    object3d_->Create();

    object3d_->SetMeshAndMaterial(circleMesh_.get());
    object3d_->SetTextureHandle(TextureFactory::NUMBERS);

    object3d_->SetLightMode(Object3d::kLightModeNone);
}

void Field::Init()
{
    object3d_->Initialize();
}

void Field::Update()
{
    uvTranslate_ += TimeManager::DeltaTime();
    object3d_->GetUVTransform().translate.y = uvTranslate_;
    object3d_->UpdateUV();
    object3d_->Update();

    DebugUI::CheckObject3d(*object3d_, "fieldObject");
}

void Field::Draw(Camera& camera)
{
    object3d_->Draw(camera);
}
