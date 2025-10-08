#pragma once  

#pragma region //MP3  

#include <vector> 
#include <mfapi.h>  
#include <mfidl.h>  
#include <mfreadwrite.h>  
#include <string>

struct SoundData {
    std::wstring filePath;
    WAVEFORMATEX* pWaveFormat;
    std::vector<BYTE> mediaData;
};

#pragma endregion  

#include <xaudio2.h> // wavファイル用  
#include <stdint.h> // int32_t  

#include <wrl.h> // ComPtr(コムポインタ)  
#include<memory>//unique_ptr

class SoundManager {
public:

    SoundManager();
    ~SoundManager() = default;
    SoundManager(SoundManager&) = delete;

    static SoundManager* GetInstance();
    static void LoadSoundData(const std::wstring& path);
    static uint32_t Load(const std::wstring& path);
    static uint32_t GetSoundByIndex(const std::wstring& filePath);
    static void Initialize();

    static void Play(const uint32_t tag, const float& volume, bool isLoop = false);
    /// @brief 音声データの解放関数  
    /// @param soundData 音声データ  
    static void Unload(SoundData* soundData);
    static void Stop();
    static void Pause();  // 一時停止
    static void Resume(); // 再開

    static bool IsActuallyPlaying();
    static bool IsPlaying();
 
private:

    SoundManager& operator=(SoundManager&) = delete;

private:
    static SoundManager* instance_;

    static  Microsoft::WRL::ComPtr<IXAudio2> xAudio2_; // ComオブジェクトなのでComPtrで管理する。  
    static IXAudio2MasteringVoice* masterVoice_; // ReleaseなしのためComPtrで管理することが出来ない。  
    //IXAudio2SourceVoice* pSourceVoice_ = { nullptr };
    static std::vector<IXAudio2SourceVoice*> voices_;

    static bool isStarted_;
    static bool isPaused_;

    static std::vector<SoundData> soundDatas;
};