#include "CollisionConfig.h"


uint32_t CollisionTag::useIndex_ = 0;
const uint32_t CollisionTag::maxCount_ = 1000;
std::unordered_map<std::string, uint32_t>CollisionTag::tags_;

void CollisionTag::SetTag(const std::string& tagName)
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


   CollisionTag::SetTag("Player");
   CollisionTag::SetTag("PlayerBulletCold");
   CollisionTag::SetTag("PlayerBulletHot");

   CollisionTag::SetTag("Enemy");
   CollisionTag::SetTag("EnemyBulletCold");
   CollisionTag::SetTag("EnemyBulletHot");

   CollisionTag::SetTag("Medjed");
   CollisionTag::SetTag("DummyMedjed");
   CollisionTag::SetTag("Mummy");

   CollisionTag::SetTag("Wall");
   CollisionTag::SetTag("Floor");
   CollisionTag::SetTag("Water");
   
   CollisionTag::SetTag("Item");
}
