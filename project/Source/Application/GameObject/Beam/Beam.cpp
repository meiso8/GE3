#include "Beam.h"
#include"DebugUI.h"
#include"MakeMatrix.h"
#include"TimeManager.h"
#include"Easing.h"
#include"Sound.h"
#include"LineObject3d.h"

namespace {
    const float kBeamDuretion_ = 1.5f;
    const float kLigeDuration_ = 2.0f;

}

Beam::Beam()
{
    beam_ = std::make_unique<Primitive>();
    beam_->Create(PrimitiveGenerator::CreateBeam(0.5f));

    object3d_ = std::make_unique<BeamObject3d>();

    object3d_->Create();
    object3d_->SetMeshAndMaterial(beam_.get());
    object3d_->SetTextureHandle(TextureFactory::BEAM);
    object3d_->SetTemperature(1.0f);

#ifdef _DEBUG

    lineObj_ = std::make_unique<LineObject3d>();
    lineObj_->Create({ -1.0f,0.0f,0.0f }, { 1.0f,0.0f,0.0f });
    lineObj_->SetColor({ 1.0f,0.0f,0.0f,1.0f });

#endif



    type_ = kEnemy;

    CreateParticle();
}

Beam::~Beam()
{
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
    object3d_->SetLightMode(Object3d::kLightModeNone);


#ifdef _DEBUG
    lineObj_->SetVertex(ray_.origin, ray_.diff + ray_.origin);

#endif



}


void Beam::Update()
{

#ifdef USE_IMGUI

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
        lifeTimer_ -= TimeManager::DeltaTime();
    }

    float time = (kLigeDuration_ - lifeTimer_) / kBeamDuretion_;
    time = std::clamp(time, 0.0f, 1.0f);

    Vector3 pos = { 0.0f };

    if (parent_) {
        pos = Math::GetWorldTransformByMatrix(*parent_);
    } else {
        pos = startPos_;
    }

    point_ = { .startPos = pos ,.endPos = Lerp(pos, endPos_, time) };

    UpdateObject();

}

void Beam::Draw(Camera* camera)
{

#ifdef _DEBUG
    lineObj_->Draw(*camera, false);
#endif

    if (!isActive_) {
        return;
    }

    object3d_->SetLightMode(Object3d::kLightModeNone);
    object3d_->Draw(*camera);


}


bool Beam::Shot(const Vector3& target, const  BeamType& type, const Vector3& startPos, Matrix4x4* parent)
{

    endPos_ = target;
    type_ = type;

    parent_ = parent;

    Vector3 pos = { 0.0f };

    if (parent_) {
        pos = Math::GetWorldTransformByMatrix(*parent_);
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

    emitter_->GetEmitter().transform.eTransform_.translate = point_.endPos;
    emitter_->Update();

#ifdef _DEBUG
    lineObj_->SetVertex(point_.startPos, point_.endPos);
    lineObj_->Update();

#endif


    Vector3 rotate = CalculateLookAtRotate(point_.startPos, point_.endPos);
    float scaleZ = Length(point_.endPos - point_.startPos);
    Vector3 centerPos = Lerp(point_.startPos, point_.endPos, 0.5f);

    auto& transfrom = object3d_->GetTransform();
    transfrom.rotate = rotate;
    transfrom.scale = { 1.0f,1.0f,scaleZ };
    transfrom.translate = centerPos;

    object3d_->Update();
}

void Beam::CreateParticle()
{
    emitter_ = std::make_unique<ParticleEmitter>();
    emitter_->Initialize();
    emitter_->SetName("particle1");

    auto& emitter0 = emitter_->GetEmitter();

    emitter0.count = 8;
    emitter0.color = { 1.0f,0.5f,0.5f,1.0f };
    emitter0.transform.eTransform_.scale = { 0.0125f,0.25f,0.25f };
    emitter0.transform.eTransform_.rotate = { 0.0f,0.0f,0.0f };
    emitter0.transform.eTransform_.translate = { 0.0f,0.0f,0.0f };

    emitter0.frequencyTime = 0.01f;
    emitter0.frequency = 0.1f;
    emitter0.lifeTime = 0.1f;
    emitter0.blendMode = kBlendModeAdd;
    emitter0.movement = ParticleMovements::kParticleNormal;
    emitter0.rotateAABB_ = { .min = {0.0f,0.0f,-3.14f},.max = {0.0f,0.0f,3.14f} };
    emitter0.scaleAABB_ = { .min = {0.0f,0.4f,0.0f},.max = {0.0f,1.5f,1.0f} };
    emitter0.isLoop_ = true;
    emitter0.accelerationField_.area = { .min = {-1.0f,-1.0f,-1.0f},.max = {1.0f,1.0f,1.0f} };
    emitter0.useBillboard_ = true;
}
