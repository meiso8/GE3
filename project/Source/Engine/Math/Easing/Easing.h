#pragma once
#include "Lerp.h"

float NormalizeAngle(float angle);

/// @brief イーズイン t^2
/// @param t 時間
/// @return イージングされた時間
float EaseInQuadT(const float& t);
/// @brief イーズインキュービック t^3
/// @param t 時間
/// @return イージングされた時間
float EaseInCubicT(const float& t);
/// @brief イーズインクォート t^4
/// @param t 時間
/// @return イージングされた時間
float EaseInQuartT(const float& t);
/// @brief イーズインクイント t^5
/// @param t 時間
/// @return イージングされた時間
float EaseInQuIntT(const float& t);
/// @brief イーズインサイン
/// @param t 時間
/// @return イージングされた時間
float EaseInSineT(const float& t);
/// @brief イーズインサーク
/// @param t 時間
/// @return イージングされた時間
float EaseInCircT(const float& t);
/// @brief イーズインエラスティック
/// @param t 時間
/// @return イージングされた時間
float EaseInElasticT(const float& t);
/// @brief イーズインエクスポ
/// @param t 時間
/// @return イージングされた時間
float EaseInExpoT(const float& t);
/// @brief イーズインバック
/// @param t 時間
/// @return イージングされた時間
float EaseInBackT(const float& t);
/// @brief イーズインバウンス
/// @param t 時間
/// @return イージングされた時間
float EaseInBounceT(const float& t);

// ======================================================================

/// @brief イーズアウトクアッド t^2
/// @param t 時間
/// @return イージングされた時間
float EaseOutQuadT(const float& t);
/// @brief イーズアウトキュービック t^3
/// @param t 時間
/// @return イージングされた時間
float EaseOutCubicT(const float& t);
/// @brief イーズアウトクォート t^4
/// @param t 時間
/// @return イージングされた時間
float EaseOutQuartT(const float& t);
/// @brief イーズアウトクイント t^5
/// @param t 時間
/// @return イージングされた時間
float EaseOutQuintT(const float& t);
/// @brief イーズアウトサイン
/// @param t 時間
/// @return イージングされた時間
float EaseOutSineT(const float& t);
/// @brief イーズアウトサーク
/// @param t 時間
/// @return イージングされた時間
float EaseOutCircT(const float& t);
/// @brief イーズアウトエラスティック
/// @param t 時間
/// @return イージングされた時間
float EaseOutElasticT(const float& t);
/// @brief イーズアウトエクスポ
/// @param t 時間
/// @return イージングされた時間
float EaseOutExpoT(const float& t);
/// @brief イーズアウトバック
/// @param t 時間
/// @return イージングされた時間
float EaseOutBackT(const float& t);
/// @brief イーズアウトバウンス
/// @param t 時間
/// @return イージングされた時間
float EaseOutBounceT(const float& t);

// ============================//イーズインアウト==========================================


//イーズインアウト
float EaseInOutT(const float& t);
/// @brief イーズインアウトクアッド t^2
/// @param t 時間
/// @return イージングされた時間
float EaseInOutQuadT(const float& t);
/// @brief イーズインアウトキュービック t^3
/// @param t 時間
/// @return イージングされた時間
float EaseInOutCubicT(const float& t);
/// @brief イーズインアウトクォート t^4
/// @param t 時間
/// @return イージングされた時間
float EaseInOutQuartT(const float& t);
/// @brief イーズインアウトサーク
/// @param t 時間
/// @return イージングされた時間
float EaseInOutCircT(const float& t);
/// @brief イーズインアウトクイント t^5
/// @param t 時間
/// @return イージングされた時間
float EaseInOutQuintT(const float& t);
/// @brief イーズインアウトサイン
/// @param t 時間
/// @return イージングされた時間
float EaseInOutSineT(const float& t);
/// @brief イーズインアウトエラスティック
/// @param t 時間
/// @return イージングされた時間
float EaseInOutElasticT(const float& t);
/// @brief イーズインアウトエクスポ
/// @param t 時間
/// @return イージングされた時間
float EaseInOutExpoT(const float& t);
/// @brief イーズインアウトバック
/// @param t 時間
/// @return イージングされた時間
float EaseInOutBackT(const float& t);
/// @brief イーズインアウトバウンス
/// @param t 時間
/// @return イージングされた時間
float EaseInOutBounceT(const float& t);


// ============================//線形補間をかませた始点と終点を指定するイージング==========================================

class Easing {

public:
    /// @brief イーズインクアッド t^3
    template <typename T> static T EaseInQuad(T start, T end, float t) {
        float time = EaseInQuadT(t);
        return Lerp(start, end, time);
    }

    /// @brief イーズインキュービック t^3
    template <typename T>static T EaseInCubic(T start, T end, float t) {
        float time = EaseInCubicT(t);
        return Lerp(start, end, time);
    }
    /// @brief イーズインクォート t^4
    template <typename T>static T EaseInQuart(T start, T end, float t) {
        float time = EaseInQuartT(t);
        return Lerp(start, end, time);
    }
    /// @brief イーズインクイント t^5
    template <typename T> static T EaseInQuInt(T start, T end, float t) {
        float time = EaseInQuIntT(t);
        return Lerp(start, end, time);
    }
    /// @brief イーズインサイン
    template <typename T> static T EaseInSine(T start, T end, float t) {
        float time = EaseInSineT(t);
        return Lerp(start, end, time);
    }
    /// @brief イーズインサーク
    template <typename T>static T EaseInCirc(T start, T end, float t) {
        float time = EaseInCircT(t);
        return Lerp(start, end, time);
    }
    /// @brief イーズインエラスティック
    template <typename T> static T EaseInElastic(T start, T end, float t) {
        float time = EaseInElasticT(t);
        return Lerp(start, end, time);
    }
    /// @brief イーズインエクスポ
    template <typename T>static  T EaseInExpo(T start, T end, float t) {
        float time = EaseInExpoT(t);
        return Lerp(start, end, time);
    }
    /// @brief イーズインバック
    template <typename T> static T EaseInBack(T start, T end, float t) {
        float time = EaseInBackT(t);
        return Lerp(start, end, time);
    }
    /// @brief イーズインバウンス
    template <typename T> static T EaseInBounce(T start, T end, float t) {
        float time = EaseInBounceT(t);
        return Lerp(start, end, time);
    }
    // ======================================================================

    /// @brief イーズアウトクアッド t^2
    template <typename T> static T EaseOutQuad(T start, T end, float t) {
        float time = EaseOutQuadT(t);
        return Lerp(start, end, time);
    }
    /// @brief イーズアウトキュービック t^3
    template <typename T>static  T EaseOutCubic(T start, T end, float t) {
        float time = EaseOutCubicT(t);
        return Lerp(start, end, time);
    }
    /// @brief イーズアウトクォート t^4
    template <typename T> static T EaseOutQuart(T start, T end, float t) {
        float time = EaseOutQuartT(t);
        return Lerp(start, end, time);
    }
    /// @brief イーズアウトクイント t^5
    template <typename T> static T EaseOutQuint(T start, T end, float t) {
        float time = EaseOutQuintT(t);
        return Lerp(start, end, time);
    }

    /// @brief イーズアウトサイン
    template <typename T> static T EaseOutSine(T start, T end, float t) {
        float time = EaseOutSineT(t);
        return Lerp(start, end, time);
    }

    /// @brief イーズアウトサーク
    template <typename T> static T EaseOutCirc(T start, T end, float t) {
        float time = EaseOutCircT(t);
        return Lerp(start, end, time);
    }

    /// @brief イーズアウトエラスティック
    template <typename T>static  T EaseOutElastic(T start, T end, float t) {
        float time = EaseOutElasticT(t);
        return Lerp(start, end, time);
    }

    /// @brief イーズアウトエクスポ
    template <typename T> static T EaseOutExpo(T start, T end, float t) {
        float time = EaseOutExpoT(t);
        return Lerp(start, end, time);
    }
    /// @brief イーズアウトバック
    template <typename T> static T EaseOutBack(T start, T end, float t) {
        float time = EaseOutBackT(t);
        return Lerp(start, end, time);
    }
    /// @brief イーズアウトバウンス
    template <typename T> static T EaseOutBounce(T start, T end, float t) {
        float time = EaseOutBounceT(t);
        return Lerp(start, end, time);
    }

    // ============================//イーズインアウト==========================================


    //イーズインアウト
    template <typename T> static T EaseInOut(T start, T end, float t) {
        float time = EaseInOutT(t);
        return Lerp(start, end, time);
    }
    /// @brief イーズインアウトクアッド t^2
    template <typename T> static T EaseInOutQuad(T start, T end, float t) {
        float time = EaseInOutQuadT(t);
        return Lerp(start, end, time);
    }
    /// @brief イーズインアウトキュービック t^3
    template <typename T> static T EaseInOutCubic(T start, T end, float t) {
        float time = EaseInOutCubicT(t);
        return Lerp(start, end, time);
    }
    /// @brief イーズインアウトクォート t^4
    template <typename T>static  T EaseInOutQuart(T start, T end, float t) {
        float time = EaseInOutQuartT(t);
        return Lerp(start, end, time);
    }
    /// @brief イーズインアウトサーク
    template <typename T> static T EaseInOutCirc(T start, T end, float t) {
        float time = EaseInOutCircT(t);
        return Lerp(start, end, time);
    }
    /// @brief イーズインアウトクイント t^5
    template <typename T> static T EaseInOutQuint(T start, T end, float t) {
        float time = EaseInOutQuintT(t);
        return Lerp(start, end, time);
    }
    /// @brief イーズインアウトサイン
    template <typename T> static T EaseInOutSine(T start, T end, float t) {
        float time = EaseInOutSineT(t);
        return Lerp(start, end, time);
    }
    /// @brief イーズインアウトエラスティック
    template <typename T> static T EaseInOutElastic(T start, T end, float t) {
        float time = EaseInOutElasticT(t);
        return Lerp(start, end, time);
    }
    /// @brief イーズインアウトエクスポ
    template <typename T>static  T EaseInOutExpo(T start, T end, float t) {
        float time = EaseInOutExpoT(t);
        return Lerp(start, end, time);
    }
    /// @brief イーズインアウトバック
    template <typename T>static  T EaseInOutBack(T start, T end, float t) {
        float time = EaseInOutBackT(t);
        return Lerp(start, end, time);
    }
    /// @brief イーズインアウトバウンス
    template <typename T> static T EaseInOutBounce(T start, T end, float t) {
        float time = EaseInOutBounceT(t);
        return Lerp(start, end, time);
    }
};