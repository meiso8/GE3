#pragma once
#include"../Engine/FreeTypeManager/Text.h"
class Sprite;
#include<memory>
class PasswordText
{
public:
    PasswordText();
    ~PasswordText() = default;
    void Initialize();
    void Update();
    void Draw();
    bool GetIsUnLock() { return isUnLock_; }
    static bool GetIsActive() { return isActive_; }
    static void SetIsActive(const bool isActive) { isActive_ = isActive; }
    void SetCurPosPtr(Vector2* curPosPtr) { curPosPtr_ = curPosPtr; }
private:
    //入力文字
    std::u32string inputString_;

    //パスワード
    std::u32string passworldString_;
    //実際の表示テキスト
    Text text_;
    //情報テキスト
    Text infoText_;
    //ロックが解除される
    bool isUnLock_ = false;
    static bool isActive_;
    //ウェブ用スプライト
    std::unique_ptr<Sprite>webSprite_ = nullptr;
    Vector2* curPosPtr_ = nullptr;
};

