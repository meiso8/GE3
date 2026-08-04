#include "EyeCollider.h"
#include"CollisionConfig.h"
#include"TimeManager.h"
#include"Easing.h"
#include<algorithm>
#include"MakeMatrix.h"


void EyePosition::Update()
{

    if (isUp_ || isDown_) {
        //上昇時または下降時

        EasingUpdate();

        //タイマーが1.0fになったら
        if (time_ >= 1.0f) {

            //加工時だったら
            if (isDown_) {
                time_ = 0.0f;
                isDown_ = false;
            }
        }

    } else {
        //上昇時と下降時以外は初期化しておく　回転は別
        transform_.eTransform_.translate = Math::ZERO;
    }

    //行列計算
    transform_.matWorld_ = MakeAffineMatrix(transform_.eTransform_.scale, transform_.eTransform_.rotate, transform_.eTransform_.translate) * *parent_;
}


void EyePosition::EasingUpdate()
{
    //タイマーをアップデートする
    const float speed = isDown_ ? 2.0f : 1.0f;
    time_ += speed* TimeManager::DeltaTime();
    time_ = std::clamp(time_, 0.0f, 1.0f);
    //イージングを行う
    transform_.eTransform_.translate = Easing::EaseInOutBack(startPos_, endPos_, time_);
    transform_.eTransform_.rotate = Easing::EaseInOutBack(startRot_, endRot_, time_);
}

void EyePosition::ShftToTDown()
{  
    if (isDown_) {
        //既に下降中だったらスキップする
        return;
    }

    //タイマーが1.0f未満ならスキップする
    if (time_ < 1.0f) {
        return;
    }

    //上昇時だったら
    if (isUp_) {

        isUp_ = false;
        time_ = 0.0f;

        startPos_ = transform_.eTransform_.translate;
        endPos_ = Math::ZERO;
        startRot_ = transform_.eTransform_.rotate;
        endRot_ = Math::ZERO;
        //下降開始
        isDown_ = true;
    }
}

void EyePosition::SiftToUp()
{
    if (isUp_) {
        //安全処理として既に上昇していたらスキップする
        return;
    }

    //上昇フラグをセットする
    isUp_ = true;
    //上から視点から戻る際のフラグ
    isDown_ = false;
    //時間
    time_ = 0.0f;
    startPos_ = transform_.eTransform_.translate;
    //終了位置
    endPos_ = { 0.0f ,5.0f,0.0f };
    //開始回転
    startRot_ = transform_.eTransform_.rotate;
    //終了回転
    endRot_ = {Math::kHPi,0.0f,0.0f };
}

void EyePosition::Initialize()
{
    transform_.Initialize();
    transform_.eTransform_.translate.z = 0.0f;
    transform_.eTransform_.translate.y = 0.0f;

    //上昇フラグ
    isUp_ = false;
    //上から視点から戻る際のフラグ
    isDown_ = false;
    //開始位置
    startPos_ = transform_.eTransform_.translate;
    //終了位置
    endPos_ = transform_.eTransform_.translate;
    //開始回転
    startRot_ = transform_.eTransform_.rotate;
    //終了回転
    endRot_ = transform_.eTransform_.rotate;
    //時間
    time_ = 0.0f;
}

Vector3& EyePosition::GetForward()
{
    //前方を取得する
    static Vector3 forward;
    forward = Math::GetForward(GetWorldMatrix());
    return forward;
}


void EyePosition::MouseLook(const float& rotateX)
{

    if (isUp_||isDown_) {
        //一応ここでも上昇時または下降時はスキップする
        return;
    }

    transform_.eTransform_.rotate.x = Lerp(transform_.eTransform_.rotate.x, rotateX, 0.5f);
}

