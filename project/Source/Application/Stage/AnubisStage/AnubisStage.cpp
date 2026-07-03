#include "AnubisStage.h"
#include"Sound.h"

#include"DebugUI.h"

AnubisStage::AnubisStage()
{
    backGround_ = std::make_unique<BackGround>();
    fountain_ = std::make_unique<Fountain>();
    anubisBlockMap_ = std::make_unique<AnubisBlockMap>();
}

void AnubisStage::Initialize()
{

    //ステージのロード
    LoadAndCreateObject("AnubisStage_objectEditor");

    memoManager_->GenerateMemos({ TextureFactory::BOOK5 });

    anubisBlockMap_->Initialize();
    anubisBlockMap_->Update();


    // ミイラ前に移動
    player_->Init({ 0.0f, 0.0f, -5.0f });

    Sound::StopAllSound();

    backGround_->Initialize();

    Sound::bgmVolume_ = 0.1f;

    fountain_->Initialize();
}

void AnubisStage::Update()
{
    Sound::PlayBGM(SoundFactory::BGM_Sun);

    anubisBlockMap_->Update();

    if (anubisBlockMap_->IsClear()) {

    }

    backGround_->Update();
    fountain_->Update();

    //オブジェクトの更新
    UpdateObject();
}

void AnubisStage::Draw(Camera& camera)
{
    fountain_->Draw(camera);
    backGround_->Draw(camera);
    anubisBlockMap_->Draw(camera);

    //オブジェクトの描画
    DrawObject(camera);
}


void AnubisStage::DrawSprite()
{

}

void AnubisStage::CheckCollision(CollisionManager& collisionManager)
{
    anubisBlockMap_->RayCastHit(*player_->GerRaySprite());

    // 壁との当たり判定
    for (auto& [type, object] : backGround_->GetBuilding()->GetFieldPoses()) {
        collisionManager.AddCollider(object.get());
    }

    collisionManager.AddCollider(fountain_.get());

    //マップ
    for (auto& block : anubisBlockMap_->GetMap()) {
        collisionManager.AddCollider(block.get());
    }

    //コライダーを追加する
    AddObjectCollision(collisionManager);
}

