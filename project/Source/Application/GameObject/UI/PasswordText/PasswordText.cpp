#include "PasswordText.h"
#include"FreeTypeManager/FreeTypeManager.h"
#include"Sprite.h"
#include"Input.h"
#include"SoundManager/SoundManager.h"
#include"InputBind.h"
#include"Sound.h"

 bool PasswordText::isActive_ = true;
PasswordText::PasswordText()
{
    uint32_t meiryob = FreeTypeManager::CreateFace("Resource/Fonts/meiryob.ttc", 0);
    uint32_t handle = FreeTypeManager::CreateFace("Resource/Fonts/NotoSansEgyptianHieroglyphs-Regular.ttf", 0);

    text_.Initialize(handle);
    infoText_.Initialize(meiryob);

    text_.SetString(U"");
    text_.SetPosition({ 640 - 64, 360 });
    text_.SetColor({ 1, 1, 1, 1 });
    text_.SetAlign(Text::TextAlign::Left);
    text_.SetBlendMode(BlendMode::kBlendModeNormal);

    //情報テキスト
    infoText_.SetString(U"チケット番号を入力してください");
    infoText_.SetPosition({ 640, 300 });
    infoText_.SetColor({ 1, 1, 1, 1 });
    infoText_.SetAlign(Text::TextAlign::Center);
    infoText_.SetBlendMode(BlendMode::kBlendModeNormal);

    float width = static_cast<float>(Window::GetClientWidth());

}

void PasswordText::Initialize()
{
    passworldString_ = U"A3N91KH7";
    text_.SetString(U"");
    isUnLock_ = false;
    isActive_ = false;
}

void PasswordText::Update()
{

    if (!isActive_) {
        //非アクティブの時はクリアする
        inputString_.clear();
        return;
    }

    if (isUnLock_) {

        isActive_ = false;
        //ロックが解除されたら今後処理を行わない
        return;
    }


    // Backspace　文字を消す
    if (Input::IsTriggerKey(DIK_BACK)) {
        if (!inputString_.empty()) {
            inputString_.pop_back();
            Sound::PlaySE(SoundFactory::PC_Keyboard);
        }
    }

    if (inputString_.size() >= passworldString_.size()) {
        if (inputString_ == passworldString_) {
            //チケット番号と同じ
            
           //成功音
           SoundManager::PlayCorrectSE();
           isUnLock_ = true;
       

        } else {

            //間違っている
            inputString_.clear();
            //cancel音を鳴らす
            SoundManager::PlayCancelSE();
        }
    }
    if (Input::IsAnyKeyPressed()) {
        //特別なキー以外の時に記録する
        for (char32_t ch : Input::GetInputChars()) {
            if (ch == U'\r' || ch == U'\n' || ch == U'\t' || ch == U'　' || ch == U' ' || ch == U'\b' || ch == U'\0') {

                continue;
            }
            //記録
            Sound::PlaySE(SoundFactory::PC_Keyboard);
            inputString_ += ch;
        }

    }

    text_.SetString(inputString_);


}

void PasswordText::Draw()
{
    if (!isActive_) {
        return;
    }

    Sprite::PreDraw();
    infoText_.Draw();
    text_.Draw();

}
