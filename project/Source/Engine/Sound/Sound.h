#pragma once  

#pragma region //MP3  

#include <vector> 
#include <mfapi.h>  
#include <mfidl.h>  
#include <mfreadwrite.h>  
#include <string>
#include<filesystem>

struct SoundData {
    WAVEFORMATEX pWaveFormat;
    std::vector<BYTE> mediaData;
};

#pragma endregion  

#include <xaudio2.h> // wavファイル用  
#include <stdint.h> // int32_t  

#include <wrl.h> // ComPtr(コムポインタ)  
#include<memory>//unique_ptr
#include <unordered_map>
#include"SoundFactory.h"

class Sound {
public:
    static const uint32_t kMaxSoundCount_ = 512;
public:
    /// @brief BGＭを再生する
    /// @param tag タグ
    /// @param volumeFactor 倍率
    /// @param loop ループフラグ　デフォルト true
    static void PlayBGM(const SoundFactory::TAG& tag, const float& volumeFactor = 1.0f, const bool& loop = true);
    /// @brief SEを再生する
    /// @param tag タグ
    /// @param volumeFactor  倍率
    /// @param loop ループフラグ デフォルト false
    static void PlaySE(const SoundFactory::TAG& tag, const float& volumeFactor = 1.0f, const bool& loop = false);
    /// @brief LoopSEを再生する
    /// @param tag タグ
    /// @param volumeFactor   倍率
    static void PlayLoopSE(const SoundFactory::TAG& tag, const float& volumeFactor = 1.0f);
    /// @brief 重複を省いたSEを再生する
    /// @param tag タグ
    /// @param volumeFactor  倍率
    static void PlayOriginSE(const SoundFactory::TAG& tag, const float& volumeFactor = 1.0f);

    //何も囲わない
    static void Play(const SoundFactory::TAG& tag, const float& volume, const bool& isLoop = false);
    static void Play(const std::filesystem::path& path, const float& volume, const bool& isLoop = false);


    /// @brief 一時停止
    /// @param tag 
    static void Pause(const SoundFactory::TAG& tag);
    static void Pause(const std::filesystem::path& path);
    /// @brief 再開
    /// @param tag タグ
    static void Resume(const SoundFactory::TAG& tag);
    static void Resume(const std::filesystem::path& path);
    /// @brief 停止
    /// @param tag タグ 
    static void Stop(const SoundFactory::TAG& tag);
    static void Stop(const std::filesystem::path& path);
    /// @brief 全ての音を停止する
    static void StopAllSound();
    /// @brief プレイ中かどうかを検証する
    /// @param tag タグ 
    /// @return 判定結果
    static bool IsPlaying(const SoundFactory::TAG& tag);
    static bool IsPlaying(const std::filesystem::path& path);
    /// @brief 音階の設定
    /// @param tag タグ
    /// @param ratioNum　音階の数字 
    static void SetFrequencyRatio(const SoundFactory::TAG& tag, const int ratioNum);
    /// @brief 音声データの解放関数  
/// @param soundData 音声データ  
    static void Unload(SoundData& soundData);
    static void Initialize();
    static void Finalize();
    static void SetVol(const float& vol, const SoundFactory::TAG& tag);
    static void SetVol(const float& vol, const std::filesystem::path& path);
    static std::vector<float> GetWaveform(const SoundFactory::TAG& tag);
    static std::vector<float> GetWaveform(const std::filesystem::path& path);

    static UINT64 GetSamplesPlayed(const SoundFactory::TAG& tag);

    static UINT64 GetSamplesPlayed(const std::filesystem::path& path);

    static void LoadAndMap(const std::filesystem::path& path, const SoundFactory::TAG& tag);

    static void Load(const std::filesystem::path& path);

    static std::unordered_map<std::filesystem::path, SoundData>& GetSouneDatas() { return  soundDatas_; }

    static void SetBGMVolume(const float vol) { bgmVolume_; }
    static void SetSEVolume(const float vol) { seVolume_; }
    static const float GetBGMVolume() { return bgmVolume_; }
    static const float GetSEVolume() { return seVolume_; }
private:

    static float bgmVolume_;
    static float seVolume_;

private:
    Sound() = default;
    ~Sound() = default;
    Sound(Sound&) = delete;
    Sound& operator=(Sound&) = delete;
    //音階調整
    static void SetFrequencyRatio(const std::filesystem::path& path, const int ratioNum);
    static bool IsPlayingAll();
    static bool LoadFile(const std::filesystem::path& path);
    static XAUDIO2_BUFFER GetBuffer(const std::filesystem::path& path);
private:

    static  Microsoft::WRL::ComPtr<IXAudio2> xAudio2_; // ComオブジェクトなのでComPtrで管理する。  
    static IXAudio2MasteringVoice* masterVoice_; // ReleaseなしのためComPtrで管理することが出来ない。  
    static std::unordered_map<std::filesystem::path, IXAudio2SourceVoice*> voices_;
    static std::unordered_map<std::filesystem::path, SoundData> soundDatas_;

    static std::unordered_map<uint32_t, std::filesystem::path> handleToPath_;
};