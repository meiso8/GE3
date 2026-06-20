#include "AnubisStage.h"
#include"Sound.h"
#include"../StageManager.h"

#include"DebugUI.h"

AnubisStage::AnubisStage()
{
    backGround_ = std::make_unique<BackGround>();
    fountain_ = std::make_unique<Fountain>();
}

void AnubisStage::Initialize()
{
    memoManager_->GenerateMemos({ TextureFactory::MEMO1, TextureFactory::MEMO3,TextureFactory::MEMO4,TextureFactory::BOOK4 });
    // ミイラ前に移動
    player_->Init({ 0.0f, 0.0f, -5.0f });

    Sound::StopAllSound();

    backGround_->Initialize();
    itemManager_->Init();

    Sound::bgmVolume_ = 0.1f;

    fountain_->Initialize();
}

void AnubisStage::Update()
{
    Sound::PlayBGM(SoundFactory::BGM_Sun);

    backGround_->Update();

    //ステージをMedjedStageにする
    //StageManager::GetInstance()->SetNestStage("MedjedStage");
    fountain_->Update();
}

void AnubisStage::Draw(Camera& camera)
{
    fountain_->Draw(camera);
    backGround_->Draw(camera);
}


void AnubisStage::DrawSprite()
{

}

void AnubisStage::CheckCollision(CollisionManager& collisionManager)
{
    // 壁との当たり判定
    for (auto& [type, object] : backGround_->GetBuilding()->GetFieldPoses()) {
        collisionManager.AddCollider(object.get());
    }

    collisionManager.AddCollider(fountain_.get());
}

