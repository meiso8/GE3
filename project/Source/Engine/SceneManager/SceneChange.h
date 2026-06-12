#pragma once
#include<cstdint>
#include<memory>
#include"Sprite.h"
#include<unordered_map>
#include<functional>
class SceneChange;

typedef void (SceneChange::* PFunc)();

class SceneChange
{
public:

    enum State {
        //何もない
        kUnKnown,
        //フェードイン
        kFadeIn,
        //フェードアウト
        kFadeOut,
        //ワイプイン
        kWipeIn,
        //ワイプアウト
        kWipeOut,
        //シーンの開始　1フレーム
        kSceneStart,
        //シーンの終了　1フレーム
        kSceneEnd
    };

    /// @brief シーンの状態と終了時間を設定する
    /// @param state シーンの状態
    /// @param endTime 終了時間 
    void SetState(State state, const float endTime);
    /// @brief 時間を得る
    /// @return 
    static float GetTimer() { return timer_; }
    /// @brief シーン遷移の初期化　これをシーンの初期化で呼ぶ
    void Initialize();
    /// @brief シーン遷移の描画処理
    void Draw();
    /// @brief スタート判定を得る
    /// @return 判定結果
    bool IsStartScene() { return(state_ == kSceneStart) ? true : false; }
    /// @brief エンド判定を得る
    /// @return 判定結果
    bool IsEndScene() { return(state_ == kSceneEnd) ? true : false; }
   
    bool IsStateTransition() { return state_ == kFadeIn|| state_ == kFadeOut|| state_ == kWipeIn|| state_ == kWipeOut; }

    /// @brief シーンの更新
    void Update();
    SceneChange();
    ~SceneChange();
private:
    static float timer_;
    static float endTime_;

    std::unique_ptr<Sprite> sprite_ = nullptr;
    State state_ = SceneChange::State::kUnKnown;
    //メンバ関数ポインタテーブル
    static std::unordered_map<State, PFunc>StatesUpdate_;
    void FadeOut();
    void FadeIn();
    void WipeOut();
    void WipeIn();

    void TimerUpdate();
    void SwitchScene();

};


