#include "CollisionConfig.h"


uint32_t CollisionTag::useIndex_ = 0;
const uint32_t CollisionTag::maxCount_ = 1000;
std::unordered_map<std::string, uint32_t>CollisionTag::tags_;

void CollisionTag::AddTag(const std::string& tagName)
{
    if (tags_.contains(tagName)|| useIndex_ >= maxCount_) {
        return;
    };

    useIndex_++;

    tags_[tagName] = 0b1 << useIndex_;

}

uint32_t CollisionTag::GetTag(const std::string& tagName)
{
    if (tags_.contains(tagName)) {
        return tags_.at(tagName);
    };

    //何にも属さないを 返す
    return 0;
}


void TagFactory::SetTag()
{

   CollisionTag::AddTag("Player");
   CollisionTag::AddTag("PlayerBulletCold");
   CollisionTag::AddTag("PlayerBulletHot");

   CollisionTag::AddTag("Enemy");
   CollisionTag::AddTag("EnemyBulletCold");
   CollisionTag::AddTag("EnemyBulletHot");

   CollisionTag::AddTag("Medjed");
   CollisionTag::AddTag("DummyMedjed");
   CollisionTag::AddTag("Mummy");

   CollisionTag::AddTag("Wall");
   CollisionTag::AddTag("Floor");
   CollisionTag::AddTag("Water");
   CollisionTag::AddTag("Block");

   CollisionTag::AddTag("Item");
   CollisionTag::AddTag("Memo");

   CollisionTag::AddTag("StageTrigger");
}
