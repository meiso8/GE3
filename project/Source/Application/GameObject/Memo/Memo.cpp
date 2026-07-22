#include "Memo.h"
#include"CollisionConfig.h"

Memo::Memo() {

    SetCollisionAttribute(CollisionTag::GetTag("Memo"));
    SetCollisionMask(CollisionTag::GetTag("Player"));

    SetAABB({ .min = {-0.5f,-0.5f,-0.5f}, .max = {0.5f,0.5f,0.5f} });
    cubeMesh_ = std::make_unique<Primitive>();
    cubeMesh_->Create(PrimitiveGenerator::CreateCube(GetAABB()));
    object_ = std::make_unique<Object3d>();
    object_->Create();
    object_->SetMeshAndMaterial(cubeMesh_.get());
    object_->SetTemperature(0.5f);
    SetWorldMatrix(object_->GetWorldTransform());
}

void Memo::Initialize() {
    object_->Initialize();
}

void Memo::Update() {
    object_->Update();
    object_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
}

void Memo::Draw(Camera& camera) {
    object_->Draw(camera);

}
void Memo::OnCollision(Collider* collider)
{
    OnCollisionCollider();

}

