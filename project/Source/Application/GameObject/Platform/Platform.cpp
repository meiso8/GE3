#include "Platform.h"
#include "ModelManager.h"
#include"Model.h"
#include"CollisionConfig.h"

Platform::Platform() {

    SetCollisionAttribute(CollisionTag::GetTag("Wall"));

    SetCollisionMask(
        CollisionTag::GetTag("Player")
        | CollisionTag::GetTag("Enemy")
        | CollisionTag::GetTag("Medjed")
        | CollisionTag::GetTag("DummyMedjed")
    );

    SetAABB({ { -1.5f, 0.0f, -0.5f }, { 1.5f, 0.75f, 1.0f } });

    cubeMesh_ = std::make_unique<Primitive>();
    cubeMesh_->Create(PrimitiveGenerator::CreateCube(GetAABB()));

    object_ = std::make_unique<Object3d>();
    object_->Create();// 台座用モデル
    object_->SetMeshAndMaterial(cubeMesh_.get());
    object_->SetTextureHandle(TextureFactory::PUZZLE);
    SetWorldMatrix(object_->GetWorldTransform().matWorld_);

}

void Platform::Initialize() {
    object_->Initialize();

}

void Platform::Update() {
    object_->Update();
}

void Platform::Draw() {
    object_->Draw();
   
}


void Platform::OnCollision(Collider* collider)
{

}
