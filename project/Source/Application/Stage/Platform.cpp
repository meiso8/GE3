#include "Platform.h"
#include "ModelManager.h"
#include"Model.h"
#include"CollisionConfig.h"

Platform::Platform() {

    SetCollisionAttribute(kCollisionWall);
    SetCollisionMask(kCollisionPlayer | kCollisionEnemy | kCollisionMedjed | kCollisionDummyMedjed);

    SetAABB({ { -1.5f, 0.0f, -0.5f }, { 1.5f, 0.75f, 1.0f } });

    cubeMesh_ = std::make_unique<Primitive>();
    cubeMesh_->Create(PrimitiveGenerator::CreateCube(GetAABB()));

    object_ = std::make_unique<Object3d>();
    object_->Create();
    object_->SetMesh(cubeMesh_.get()); // 台座用モデル
    object_->SetTextureHandle(TextureFactory::PUZZLE);
    SetWorldMatrix(object_->worldTransform_.matWorld_);

}

void Platform::Initialize() {
    object_->Initialize();

}

void Platform::Update() {
    object_->Update();
    ColliderUpdate();

}

void Platform::Draw(Camera& camera) {
    object_->Draw(camera);
    ColliderDraw(camera);
}


void Platform::OnCollision(Collider* collider)
{

}
