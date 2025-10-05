#include"Sound.h"
#include <fstream>
#include<assert.h>

#pragma comment(lib, "Mf.lib")  
#pragma comment(lib, "mfplat.lib")  
#pragma comment(lib, "MFreadwrite.lib")  
#pragma comment(lib, "mfuuid.lib")  

#pragma comment(lib, "xaudio2.lib") // xaudio2.libをリンクする。  


std::unique_ptr<Sound> Sound::instance_ = nullptr;

Sound::Sound() {
    HRESULT result;
    result = XAudio2Create(xAudio2_.GetAddressOf(), 0, XAUDIO2_DEFAULT_PROCESSOR);
    assert(SUCCEEDED(result));

    //マスターボイスの生成
    result = xAudio2_->CreateMasteringVoice(&masterVoice_);//masterVoiceはxAudio2の解放と同時に無効になるため自分でdeleteしない
    assert(SUCCEEDED(result));

    result = MFStartup(MF_VERSION, MFSTARTUP_NOSOCKET);
    assert(SUCCEEDED(result));
}


Sound* Sound::GetInstance()
{
    if (instance_ == nullptr) {
        instance_ = std::make_unique<Sound>();
    }
    return instance_.get();
}

SoundData Sound::SoundLoad(const std::wstring& path) {

    //ソースリーダーの作成
    IMFSourceReader* pMFSourceReader{ nullptr };
    MFCreateSourceReaderFromURL(path.c_str(), NULL, &pMFSourceReader);

    //メディアタイプの取得
    IMFMediaType* pMFMediaType{ nullptr };
    MFCreateMediaType(&pMFMediaType);
    pMFMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
    pMFMediaType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
    pMFSourceReader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, pMFMediaType);

    pMFMediaType->Release();
    pMFMediaType = nullptr;
    pMFSourceReader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, &pMFMediaType);

    //オーディオデータ形式の作成　メディアタイプからWaveForMatexを生成
    WAVEFORMATEX* waveFormat{ nullptr };
    MFCreateWaveFormatExFromMFMediaType(pMFMediaType, &waveFormat, nullptr);
    //データの読み込み
    std::vector<BYTE>mediaData;

    while (true) {
        IMFSample* pMFSample{ nullptr };
        DWORD dwStreamFlags{ 0 };
        pMFSourceReader->ReadSample(MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, nullptr, &dwStreamFlags, nullptr, &pMFSample);

        if (dwStreamFlags & MF_SOURCE_READERF_ENDOFSTREAM)
        {
            break;
        }

        IMFMediaBuffer* pMFMediaBuffer{ nullptr };
        pMFSample->ConvertToContiguousBuffer(&pMFMediaBuffer);

        BYTE* pBuffer{ nullptr };
        DWORD cbCurrentLength{ 0 };
        pMFMediaBuffer->Lock(&pBuffer, nullptr, &cbCurrentLength);

        mediaData.resize(mediaData.size() + cbCurrentLength);
        memcpy(mediaData.data() + mediaData.size() - cbCurrentLength, pBuffer, cbCurrentLength);

        pMFMediaBuffer->Unlock();

        pMFMediaBuffer->Release();
        pMFSample->Release();

    }

    //読み込んだ音声ファイルデータをreturn
    SoundData soundDataMP3 = {};

    soundDataMP3.pWaveFormat = waveFormat;
    soundDataMP3.mediaData = mediaData;

    return soundDataMP3;
}


void Sound::SoundUnload(SoundData* soundData) {

    // メディアデータの解放
    soundData->mediaData.clear();
    soundData->mediaData.shrink_to_fit();
    // waveFormatの解放
    if (soundData->pWaveFormat) {
        //CoTaskMemFree(soundData->pWaveFormat);
        soundData->pWaveFormat = nullptr;
    }

};

void Sound::SoundPlay(const SoundData& soundData, const float& volume, bool isLoop) {
    HRESULT result;

    IXAudio2SourceVoice* newVoice = nullptr;

    result = xAudio2_->CreateSourceVoice(&newVoice, soundData.pWaveFormat);
    assert(SUCCEEDED(result));
    newVoice->SetVolume(volume);

    XAUDIO2_BUFFER buf{};
    buf.pAudioData = soundData.mediaData.data();
    buf.AudioBytes = /*sizeof(BYTE) * */static_cast<UINT32>(soundData.mediaData.size());
    buf.Flags =  XAUDIO2_END_OF_STREAM;

    if (isLoop) {
        buf.LoopCount = XAUDIO2_LOOP_INFINITE; 
    }

    result = newVoice->SubmitSourceBuffer(&buf);
    assert(SUCCEEDED(result));
    result = newVoice->Start();//再生開始
    assert(SUCCEEDED(result));
    isStarted_ = true;
    isPaused_ = false;

    voices_.push_back(newVoice);

};

void Sound::SoundStop() {

    for (auto voice : voices_) {
        voice->Stop();
        voice->Discontinuity();
        voice->FlushSourceBuffers();
        voice->DestroyVoice();
    }
    voices_.clear();

    isStarted_ = false;
    isPaused_ = true;

}

void Sound::SoundPause() {
    //if (pSourceVoice_) {
    //    pSourceVoice_->Stop(); // バッファは保持されたまま停止
    //    isPaused_ = true;
    //}
}

void Sound::SoundResume() {
    //if (pSourceVoice_) {
    //    pSourceVoice_->Start(); // 停止した位置から再開
    //}
}

bool Sound::IsActuallyPlaying() const {
    return isStarted_ && !isPaused_;
}

bool Sound::IsPlaying() const {
    for (auto voice : voices_) {
        if (voice) {
            XAUDIO2_VOICE_STATE state{};
            voice->GetState(&state);
            if (state.BuffersQueued > 0) {
                return true; // 少なくとも1つの音声が再生中
            }
        }
    }
    return false; // すべての音声が停止している
}


Sound::~Sound() {
 
};