#include "AmenStage.h"

#include"SoundManager/SoundManager.h"
#include"Sound.h"
#include"../StageManager/StageManager.h"

AmenStage::AmenStage()
{
    slidePuzzleSystem_ = std::make_unique<SlidePuzzleSystem>();
    amenRa_ = std::make_unique<AmenRa>();
    backGround_ = std::make_unique<BackGround>();
}

void AmenStage::Initialize()
{
    //アイテムの生成を初期化する
    isGenerateItem_ = false;

    //ステージのロード
    LoadAndCreateObject("AmenStage_objectEditor");

    memoManager_->GenerateMemos({ TextureFactory::MEMO1, TextureFactory::MEMO3,TextureFactory::MEMO4,TextureFactory::BOOK4 });
    // ミイラ前に移動
    player_->Init({ 0.0f, 0.0f, -5.0f });

    Sound::StopAllSound();
    slidePuzzleSystem_->Initialize();
    backGround_->Initialize();

    amenRa_->Initialize();
    Sound::bgmVolume_ = 0.1f;

}

void AmenStage::Update()
{
    Sound::PlayBGM(SoundFactory::BGM_Sun);
    slidePuzzleSystem_->Update(*uiManager_->GetCurPosPtr());
    amenRa_->Update();
    backGround_->Update();

    //オブジェクトの更新
    UpdateObject();

    if (!isGenerateItem_ && slidePuzzleSystem_->GetIsGameEnd()) {
        //太陽円盤を生成する
        itemManager_->GenerateItems({ "SolarDisc" });
        isGenerateItem_ = true;
    }

}

void AmenStage::Draw(Camera& camera)
{
    backGround_->Draw(camera);
    amenRa_->Draw(camera);
    slidePuzzleSystem_->Draw(camera);
    //オブジェクトの描画
    DrawObject(camera);
}

void AmenStage::DrawSprite()
{
    slidePuzzleSystem_->DrawUI();
}


void AmenStage::CheckCollision(CollisionManager& collisionManager)
{
    if (!isGenerateItem_) {
        //アイテムを生成していないときスライドパズルのレイキャストをヒットさせる
        slidePuzzleSystem_->RayCastHit(*player_->GerRaySprite());
    }

    collisionManager.AddCollider(slidePuzzleSystem_->GetPuzzleObj());
    collisionManager.AddCollider(amenRa_.get());

    // 壁との当たり判定
    for (auto& [type, object] : backGround_->GetBuilding()->GetFieldPoses()) {
        collisionManager.AddCollider(object.get());
    }

    //コライダーを追加する
    AddObjectCollision(collisionManager);

}



