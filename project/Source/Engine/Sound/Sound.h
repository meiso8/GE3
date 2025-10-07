#pragma once  

#pragma region //MP3  

#include <vector> 
#include <mfapi.h>  
#include <mfidl.h>  
#include <mfreadwrite.h>  
#include <string>
#include<vector>
struct SoundData {
    WAVEFORMATEX* pWaveFormat;
    std::vector<BYTE> mediaData;
};

#pragma endregion  


#include <xaudio2.h> // wavファイル用  
#include <stdint.h> // int32_t  

#include <wrl.h> // ComPtr(コムポインタ)  
#include<memory>//unique_ptr

class Sound {
public:


    Sound();
    ~Sound() = default;
public:

    static Sound* GetInstance();
    SoundData SoundLoad(const std::wstring& path);

    void SoundPlay(const SoundData& soundData, const float& volume, bool isLoop = false);
    /// @brief 音声データの解放関数  
    /// @param soundData 音声データ  
    void SoundUnload(SoundData* soundData);
    void SoundStop();
    void SoundPause();  // 一時停止
    void SoundResume(); // 再開

    bool IsActuallyPlaying() const;

    bool IsPlaying()const;
 
private:
    static Sound* instance_;

    Microsoft::WRL::ComPtr<IXAudio2> xAudio2_ = nullptr; // ComオブジェクトなのでComPtrで管理する。  
    IXAudio2MasteringVoice* masterVoice_ = { nullptr }; // ReleaseなしのためComPtrで管理することが出来ない。  
    //IXAudio2SourceVoice* pSourceVoice_ = { nullptr };
    std::vector<IXAudio2SourceVoice*> voices_;

    bool isStarted_ = false;
    bool isPaused_ = false;
};