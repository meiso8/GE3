#include "WaterStage.h"
#include"SoundManager/SoundManager.h"
#include"Sound.h"
#include"../StageManager.h"

const bool WaterStage::IsClear()
{
    //金の心臓を手に入れたとき
    return (blockMap_->IsClear() && itemManager_->HasItem("GoldHeart") && itemManager_->GetItem("GoldHeart")->isGetAnimEnd_);
}
WaterStage::WaterStage()
{
    water_ = std::make_unique<Water>();
    papyrusWall_ = std::make_unique<PapyrusWall>();
    blockMap_ = std::make_unique<BlockMap>();

}

void WaterStage::Initialize()
{

    //ステージのロード
    LoadAndCreateObject("WaterStage_objectEditor");

    memoManager_->GenerateMemos({ TextureFactory::MEMO2, TextureFactory::BOOK2 });
    water_->Initialize();
    papyrusWall_->Init();
    papyrusWall_->Update();
    blockMap_->Initialize();
    blockMap_->Update();

    player_->Init({ 0.0f,0.0f, -10.0f });

    itemApper_ = false;
}

void WaterStage::Update()
{
    Sound::PlayBGM(SoundFactory::BGM_Sea);
    blockMap_->Update();

    if (blockMap_->IsClear()) {
        //クリアしていたら水が引ける
        water_->SetIsDrain(true);
        if (!itemApper_) {
            itemApper_ = true;
            itemManager_->GenerateItems({ "GoldHeart" });
        }
        
    }

    water_->Update();
    papyrusWall_->Update();

    //オブジェクトの更新
    UpdateObject();

    if (IsClear()) {
        //クリアしていたらミイラステージになる
        StageManager::GetInstance()->SetNestStage("MummyStage");
    }

}

void WaterStage::Draw(Camera& camera)
{
    papyrusWall_->Draw(camera);
    blockMap_->Draw(camera);
    water_->Draw(camera);
    //オブジェクトの描画
    DrawObject(camera);
}

void WaterStage::CheckCollision(CollisionManager& collisionManager)
{

    blockMap_->RayCastHit(*player_->raySprite_);
    //Waterのかべ
    for (auto& [type, object] : papyrusWall_.get()->GetFieldPoses()) {
        collisionManager.AddCollider(object.get());
    }

    //マップ
    for (auto& y : blockMap_->GetMap()) {
        for (auto& x : y) {
            collisionManager.AddCollider(x.get());
        }
    }

    collisionManager.AddCollider(water_.get());

    //コライダーを追加する
    AddObjectCollision(collisionManager);
}

