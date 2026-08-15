#include "WaterStage.h"
#include"SoundManager/SoundManager.h"
#include"Sound.h"

WaterStage::WaterStage()
{
    water_ = std::make_unique<Water>();
    papyrusWall_ = std::make_unique<PapyrusWall>();
    blockMap_ = std::make_unique<BlockMap>();
}

void WaterStage::Initialize()
{
    StageTransitionInitialize();

    if (isInitialize_) {
        return;
    }
    water_->Initialize();

    papyrusWall_->Init();
    papyrusWall_->Update();
    blockMap_->Initialize();
    blockMap_->Update();


    itemApper_ = false;

    isInitialize_ = true;
}

void WaterStage::StageTransitionInitialize()
{
    //ステージのロード
    LoadAndCreateObject("WaterStage_objectEditor");

    memoManager_->GenerateMemos({ TextureFactory::MEMO2, TextureFactory::BOOK2 });

    player_->Init({ 0.0f,0.0f, -10.0f });
}

void WaterStage::Update()
{

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
}

void WaterStage::Draw(Camera& camera)
{
    papyrusWall_->Draw(camera);
    blockMap_->Draw(camera);
    //オブジェクトの描画
    DrawObject(camera);
    water_->Draw(camera);
 
}

void WaterStage::CheckCollision(CollisionManager& collisionManager)
{

    blockMap_->RayCastHit(*player_->GerRaySprite());
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

