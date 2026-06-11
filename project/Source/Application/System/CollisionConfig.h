#pragma once
#include <cstdint>

namespace {
    // プレイヤー陣営
    constexpr uint32_t kCollisionPlayer = 0b1;

    // 敵陣営
    constexpr uint32_t kCollisionEnemy = 0b1 << 2;
    // 敵陣営
    constexpr uint64_t kCollisionEnemyBullet = 0b1 << 3;
    // 敵陣営
    constexpr uint32_t kCollisionMedjed = 0b1 << 4;
    // 壁
    constexpr uint64_t kCollisionWall = 0b1 << 5;
    // ダミーメジェド
    constexpr uint64_t kCollisionDummyMedjed = 0b1 << 6;
    // アイテム
    constexpr uint64_t kCollisionItem = 0b1 << 7;
    
    // プレイヤー陣営
    constexpr uint32_t kCollisionPlayerEye = 0b1 << 8;

    //みいら
    constexpr uint64_t kCollisionMummy = 0b1 << 9;
    // 水
    constexpr uint64_t kCollisionWater = 0b1 << 10;

    // 壁
    constexpr uint64_t kCollisionFloor = 0b1 << 11;

    // プレイヤー陣営
    constexpr uint32_t kCollisionPlayerBulletCold = 0b1 << 12;
    // プレイヤー陣営
    constexpr uint32_t kCollisionPlayerBulletHot = 0b1 << 13;

    // 何の陣営にも属さない
    constexpr uint32_t kCollisionNone = 0b0;

}
