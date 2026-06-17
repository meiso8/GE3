#pragma once
#include<map>
#include<string>
#include"NodeAnimation.h"
#include"Lerp.h"
#include<unordered_map>
#include<cassert>
#include<map>
#include<filesystem>

struct Skeleton;
struct Animation {
    float duration;//アニメーション全体の尺
    //NodeAnimationの集合。Node名でひけるようにしておく
    std::map<std::string, NodeAnimation> nodeAnimations;
};


// Vector3用のLerp
Vector3 Interpolate(const Vector3& a, const Vector3& b, float t);
// Quaternion用のSlerp
Quaternion Interpolate(const Quaternion& a, const Quaternion& b, float t);

template <typename T> T CalculateValue(const  std::vector<Keyframe<T>>& keyframes, const float& time) {

    assert(!keyframes.empty());//キーがない
    if (keyframes.size() == 1 || time <= keyframes[0].time) {
        return static_cast<T>(keyframes[0].value);
    }


    for (size_t index = 0; index < keyframes.size() - 1; ++index) {
        size_t nextIndex = index + 1;
        //indexとnextIndexの2つのkeyframeを取得して範囲内二次国があるかを判別
        if (keyframes[index].time <= time && time <= keyframes[nextIndex].time) {
            // 範囲内に補間する
            float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
            return static_cast<T>(Interpolate(keyframes[index].value, keyframes[nextIndex].value, t));
        }
    }

    //最後まで行っちゃっった場合は最後の値を返すよ
    return static_cast<T>((*keyframes.rbegin()).value);
};


void ApplyAnimation(Skeleton& skeleton, const Animation& animation, const float animationTime);

class AnimationManager {
public:
    static  std::map<std::string, Animation>& LoadAnimation(const std::filesystem::path& filePath);
    ~AnimationManager();
private:
    //ファイルpathとアニメーション名を指定するよ
    static std::unordered_map <std::filesystem::path, std::map<std::string, Animation>> animations_;
};
