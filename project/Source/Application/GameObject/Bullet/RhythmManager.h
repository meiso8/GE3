#pragma once
#include"../Loader/SoundFactory.h"

#include<vector>
struct RhythmBeat {
    float time;     // ビートの発生時間（秒）
    bool hit;       // すでに処理されたか
};

class RhythmManager {
public:
    void SetSound(const SoundFactory::TAG tag) { tag_ = tag; };
    void Initialize();
    void Update();
    void AddBeat(float time); // 任意のタイミングでビートを追加
    bool IsOnBeat(float currentTime, float tolerance = 0.2f); // 許容範囲内かチェック
    const float kEndSoundTime_ = 66.0f;
private:
    std::vector<RhythmBeat> beats_;
    float timer_ = 0.0f;
    SoundFactory::TAG tag_;
};
