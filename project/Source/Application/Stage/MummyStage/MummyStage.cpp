#include "MummyStage.h"

#include"SoundManager/SoundManager.h"
#include"InputBind.h"
#include<algorithm>
#include"TimeManager.h"
#include"Sound.h"

MummyStage::MummyStage()
{
    papyrus_ = std::make_unique<Papyrus>();
    mummy_ = std::make_unique<Mummy>();
    mummyRoom_ = std::make_unique<MummyRoom>();

    for (auto& dummyMummy : dummyMummies_) {
        dummyMummy = std::make_unique<DummyMummy>();
    }

    DummyMummy::SetTargetPosPtr(&player_->GerRaySprite()->ray_.origin);
}

MummyStage::~MummyStage()
{
}

void MummyStage::TimerUpdate()
{
    heartSetEndTime_ -= TimeManager::DeltaTime();
    heartSetEndTime_ = std::clamp(heartSetEndTime_, 0.0f, maxTime_);

}

void MummyStage::Initialize() {


    StageTransitionInitialize();

    if (isInitialize_) {
        return;
    }

    heartSetEndTime_ = maxTime_;

    mummyRoom_->Init();
    mummyRoom_->Update();

    papyrus_->Initialize();
    mummy_->Initialize();



    const float distance = 7.0f;
    const float spacing = 2.5f;

    for (int i = 0; i < kMaxMummy_; ++i) {
        dummyMummies_[i]->Initialize();

        Vector3 pos;
        float rotate;

        if (i < 5) {
            // 北壁
            pos = { -spacing * 2.0f + i * spacing, 0.0f, -distance };
            rotate = 0.0f;
        } else if (i < 9) {
            // 東壁
            pos = { distance, 0.0f, -distance + (i - 5) * spacing + spacing };
            rotate = -1.57f;
        } else if (i < 13) {
            // 西壁

            pos = { -distance, 0.0f, -distance + (i - 9) * spacing + spacing };
            rotate = 1.57f;
        }

        dummyMummies_[i]->SetPosition(pos);
        dummyMummies_[i]->SetRotateY(rotate);
        dummyMummies_[i]->Update();
    }

    isInitialize_ = true;

}

void MummyStage::StageTransitionInitialize()
{


    Sound::Stop(SoundFactory::BGM_Sea);
    Sound::PlaySE(SoundFactory::HORROR1);

    //ステージのロード
    LoadAndCreateObject("MummyStage_objectEditor");

    memoManager_->GenerateMemos({ TextureFactory::BOOK,TextureFactory::MEMO5 });
    // ミイラ前に移動
    player_->Init({ 0.0f, 0.0f, -5.0f });

}

void MummyStage::Update() {


    auto item = itemManager_->GetItem("GoldHeart");

    if (itemManager_ && item && item->IsUsed()) {
        //メジェドあらわる
        TimerUpdate();
    };


    if (mummy_->GetIsOpen()) {
        //棺が空いたら
        for (auto& dummyMummy : dummyMummies_) {
            if (!dummyMummy->GetIsOpen()) {
                dummyMummy->SetIsOpen(true);
                dummyMummy->SetCollisionType();
            }
        }
    }

    for (auto& dummyMummy : dummyMummies_) {

        dummyMummy->Update();
    }

    papyrus_->Update();
    mummy_->Update();
    mummyRoom_->Update();
    //オブジェクトの更新
    UpdateObject();
}

bool MummyStage::IsRayCastHit(RaySprite& raySprite)
{

    if (raySprite.Intersect( mummy_.get())) {

        if (InputBind::IsClick()) {
            //Openしていなかったらmummyをあける
            if (!mummy_->GetIsOpen()) {
                SoundManager::PlayCorrectSE();
                mummy_->SetIsOpen(true);
            }
        }


        return true;
    }

    return false;
}

void MummyStage::CheckCollision(CollisionManager& collisionManager)
{

    if (IsRayCastHit(*player_->GerRaySprite())) {

        //オープンし終わったら
        if (mummy_->GetIsOpenEnd()) {
            const Vector3 mummyPos = GetMummy()->GetWorldTransform().GetWorldPosition();
            Vector3 endPosOffset_ = { -0.3f,0.3f,0.01f };
            Vector3 startPosOffset_ = { -0.3f,0.5f,0.01f };

            //心臓を使う
            itemManager_->UseItemFromSlot(mummyPos+ startPosOffset_, mummyPos+ endPosOffset_, "GoldHeart"
            );
        }

    }


    for (auto& dummyMummy : dummyMummies_) {
        if (!dummyMummy->GetIsHitCollision())
            collisionManager.AddCollider(dummyMummy.get());
    }

    //ミイラの台も一緒に
    collisionManager.AddCollider(GetMummy());
    collisionManager.AddCollider(GetMummy()->GetPlatform());
    collisionManager.AddCollider(papyrus_.get());

    //Waterのかべ
    for (auto& [type, object] : mummyRoom_.get()->GetFieldPoses()) {
        collisionManager.AddCollider(object.get());
    }

    //コライダーを追加する
    AddObjectCollision(collisionManager);
}

void MummyStage::Draw(Camera& camera) {

    mummyRoom_->Draw(camera);
    papyrus_->Draw(camera);

    for (auto& dummyMummy : dummyMummies_) {
        dummyMummy->Draw(camera);
    }

    mummy_->Draw(camera);
    //オブジェクトの描画
    DrawObject(camera);

}
