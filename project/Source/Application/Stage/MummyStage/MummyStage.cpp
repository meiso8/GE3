#include "MummyStage.h"

#include"SoundManager/SoundManager.h"
#include"InputBind.h"
#include<algorithm>
#include"TimeManager.h"
#include"Sound.h"
#include"../StageManager.h"

MummyStage::MummyStage()
{
    papyrus_ = std::make_unique<Papyrus>();
    mummy_ = std::make_unique<Mummy>();
    mummyRoom_ = std::make_unique<MummyRoom>();

    for (auto& dummyMummy : dummyMummies_) {
        dummyMummy = std::make_unique<DummyMummy>();
    }
}



const bool MummyStage::IsClear() const
{
    //ハートタイムが0以下になったら。
    return heartSetEndTime_ <= 0.0f;;
}

void MummyStage::TimerUpdate()
{
    heartSetEndTime_ -= Time::DeltaTime();
    heartSetEndTime_ = std::clamp(heartSetEndTime_, 0.0f, maxTime_);

}

void MummyStage::Initialize() {


    memoManager_->GenerateMemos({ TextureFactory::BOOK,TextureFactory::MEMO5 });

    DummyMummy::SetTargetPosPtr(&player_->GetEyePos());

    Sound::Stop(SoundFactory::BGM_Sea);
    Sound::PlaySE(SoundFactory::HORROR1);
    heartSetEndTime_ = maxTime_;
    papyrus_->Initialize();
    mummy_->Initialize();
    mummyRoom_->Init();
    mummyRoom_->Update();
    player_->Init();
    player_->SetBodyPos({ 0.0f, 0.0f, -5.0f }); // ミイラ前に移動
    player_->Update();

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

}

void MummyStage::Update() {


    auto item = itemManager_->GetItem("GoldHeart");

    if (itemManager_ && item && item->isUsed_) {
        //メジェドあらわる
        TimerUpdate();

        if (IsClear()) {
            //クリアしたらメジェドステージに行く
            StageManager::GetInstance()->SetNestStage("MedjedStage");
        }

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

}

bool MummyStage::IsRayCastHit(RaySprite& raySprite)
{

    AABB aabb = GetAABBWorldPos(mummy_.get());

    if (raySprite.IntersectsAABB(aabb, mummy_->GetWorldTransform().GetWorldPosition())) {

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


    if (IsRayCastHit(*player_->raySprite_)) {
        //オープンし終わったら
        if (mummy_->GetIsOpenEnd()) {
            //心臓を使う
            itemManager_->UseItemFromSlot(GetMummy()->GetWorldTransform().GetWorldPosition());
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

}

void MummyStage::Draw(Camera& camera) {

    mummyRoom_->Draw(camera);
    papyrus_->Draw(camera);

    for (auto& dummyMummy : dummyMummies_) {
        dummyMummy->Draw(camera);
    }

    mummy_->Draw(camera);


}
