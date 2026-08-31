#include "World.h"
#include"Model.h"
#include"Camera.h"
#include"Input.h"
#include"Texture.h"
#include"TimeManager.h"
World::World() {
    sphereMesh_ = std::make_unique<Primitive>();
    sphereMesh_->Create(PrimitiveGenerator::CreateSphere({ .center = {0.0f},.radius = {0.5f} }, TextureFactory::WORLD));

    object3d_ = std::make_unique<Object3d>();

    object3d_->Create();
    object3d_->SetMeshAndMaterial(sphereMesh_.get());
    object3d_->SetLightMode(Object3d::kLightModeNone);

};

void World::Init()
{

    object3d_->SetTextureHandle(TextureFactory::WORLD);
    object3d_->SetColor({ 0.0f,0.0f,0.0f,1.0f });
    object3d_->Initialize();
    object3d_->SetScale({ 500.0f,500.0f,500.0f });
    timer_ = 0.0f;
}

void World::Update()
{
    object3d_->GetUVTransform().translate.x += std::numbers::pi_v<float> *0.0625f * 0.5f * TimeManager::DeltaTime();
    object3d_->UpdateUV();
    object3d_->Update();
}

void World::Draw()
{
    object3d_->Draw(kBlendModeNormal, kCullModeFront);
}

void World::SetTexture(const uint32_t& texture)
{
    if (texture == NORMAL) {
        object3d_->SetTextureHandle(TextureFactory::WORLD);
    } else {
        object3d_->SetTextureHandle(TextureFactory::Handle(texture));
    }
}

void World::UpdateColor()
{
    if (timer_ == 1.0f) {
        return;
    }

    if (timer_ < 1.0f) {
        timer_ += TimeManager::DeltaTime();
    } else {
        timer_ = 1.0f;
    }

    object3d_->SetColor({ timer_,timer_,timer_,1.0f });

}
