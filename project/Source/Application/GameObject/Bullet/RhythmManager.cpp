#include "RhythmManager.h"
#include"TimeManager.h"
#include"JsonFile.h"
#include"DebugUI.h"
#include"Sound.h"

void RhythmManager::Initialize() {

    timer_ = 0.0f;
    beats_.clear();

    Json file = JsonFile::GetJsonFiles("notes");

    for (float t : file["beats"]) {AddBeat(t); }

    // 例：4拍子で1秒ごとにビートを追加
    for (int i = 0; i < 66; ++i) {
        beats_.push_back({ i * 1.0f, false });
    }
}

void RhythmManager::Update() {

    timer_ += Time::DeltaTime();

    if (!Sound::IsPlaying(tag_)) {
        //音が鳴ってなかったら初期化する
        Initialize();
    }

#ifdef USE_IMGUI
    ImGui::SliderFloat("timer", &timer_, 0.0f, 100.0f);

#endif // USE_IMGUI

}

void RhythmManager::AddBeat(float time)
{
    beats_.push_back({ time, false });
}


bool RhythmManager::IsOnBeat(float currentTime, float tolerance) {

    for (auto& beat : beats_) {
        if (!beat.hit && std::abs(currentTime - beat.time) <= tolerance) {
            beat.hit = true;
            return true;
        }
    }
    return false;
}
