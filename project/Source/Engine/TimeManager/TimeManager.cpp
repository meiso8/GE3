#include"TimeManager.h"
#include"Log.h"
#include <thread>

float TimeManager::deltaTime_ = 1.0f / 60.0f;

TimeManager::TimeManager()
{
    reference_ = std::chrono::steady_clock::now();
    LogFile::Log("Create TimeManager FixFPS");
}

void TimeManager::Update()
{  //1/60秒ピッタリ
    const std::chrono::microseconds kMinTime(uint64_t(1000000.0f / 60.0f));
    //1/60秒よりわずかに短い時間
    const std::chrono::microseconds kMinCheckTime(uint64_t(1000000.0f / 65.0f));
    //現在時間を取得する
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    //前回記録からの経過時間を取得する
    std::chrono::microseconds elapsed =
        std::chrono::duration_cast<std::chrono::microseconds>(now - reference_);


    if (elapsed < kMinCheckTime) {
        //1/60秒経過するまで微小なスリープを繰り返す
        while (std::chrono::steady_clock::now() - reference_ < kMinTime) {
            //1μ秒スリープ
            std::this_thread::sleep_for(std::chrono::microseconds(1));
        }
    }


    // スリープを抜けた現在時間を確定させる
    std::chrono::steady_clock::time_point frame_end = std::chrono::steady_clock::now();

    // 実際に1フレームにかかった時間を「秒単位のfloat」で取得
    deltaTime_ = std::chrono::duration<float>(frame_end - reference_).count();

    //現在の時間を記録する
    reference_ = frame_end;
}

float TimeManager::DeltaTime()
{
    return deltaTime_;
}

