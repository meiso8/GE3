#include "AmenStage.h"

#include"SoundManager/SoundManager.h"
#include"Sound.h"
#include"../StageManager.h"


const bool AmenStage::IsClear()
{
    return slidePuzzleSystem_->GetIsGameEnd();
}

AmenStage::AmenStage()
{
    slidePuzzleSystem_ = std::make_unique<SlidePuzzleSystem>();
    amenRa_ = std::make_unique<AmenRa>();
    backGround_ = std::make_unique<BackGround>();
}

void AmenStage::Initialize()
{

    memoManager_->GenerateMemos({ TextureFactory::MEMO1, TextureFactory::MEMO3,TextureFactory::MEMO4,TextureFactory::BOOK4 });

    player_->Init();
    player_->SetBodyPos({ 0.0f,0.0f,-5.0f });

    Sound::StopAllSound();
    slidePuzzleSystem_->Initialize();
    backGround_->Initialize();
    itemManager_->Init();
    amenRa_->Initialize();
    Sound::bgmVolume_ = 0.1f;
}

void AmenStage::Update()
{
    Sound::PlayBGM(SoundFactory::BGM_Sun);
    slidePuzzleSystem_->Update(*uiManager_->GetCurPosPtr());
    amenRa_->Update();
    backGround_->Update();

    if (IsClear()) {
        //ステージを水にする
        StageManager::GetInstance()->SetNestStage("WaterStage");
    }
}

void AmenStage::Draw(Camera& camera)
{
    backGround_->Draw(camera);
    amenRa_->Draw(camera);
    slidePuzzleSystem_->Draw(camera);
}

void AmenStage::DrawSprite()
{
    slidePuzzleSystem_->DrawUI();
}


void AmenStage::CheckCollision(CollisionManager& collisionManager)
{
    slidePuzzleSystem_->RayCastHit(*player_->raySprite_);

    collisionManager.AddCollider(slidePuzzleSystem_->GetPuzzleObj());
    collisionManager.AddCollider(amenRa_.get());
    
        // 壁との当たり判定
    for (auto& [type, object] : backGround_->GetBuilding()->GetFieldPoses()) {
        collisionManager.AddCollider(object.get());
    }
}



