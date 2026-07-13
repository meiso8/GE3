#include"Sound.h"
#include"DirectXCommon.h"
#include <fstream>
#include<assert.h>
#include"StringUtility.h"
#include<algorithm>

#pragma comment(lib, "Mf.lib")  
#pragma comment(lib, "mfplat.lib")  
#pragma comment(lib, "MFreadwrite.lib")  
#pragma comment(lib, "mfuuid.lib")  

#pragma comment(lib, "xaudio2.lib") // xaudio2.libをリンクする。  
#include"Log.h"

using namespace Microsoft::WRL;
namespace {
    const float GetRatio(int x) { return std::pow(2.0f, x / 12.0f); };
}


ComPtr<IXAudio2> Sound::xAudio2_ = nullptr; // ComオブジェクトなのでComPtrで管理する。  

IXAudio2MasteringVoice* Sound::masterVoice_ = nullptr;

std::unordered_map<std::filesystem::path, SoundData> Sound::soundDatas_;
std::unordered_map<std::filesystem::path, IXAudio2SourceVoice*>  Sound::voices_;
std::unordered_map<uint32_t, std::filesystem::path> Sound::handleToPath_;

float Sound::bgmVolume_ = 0.25f;
float Sound::seVolume_ = 0.75f;

void Sound::PlayBGM(const SoundFactory::TAG& tag, const float& volumeOffset, const bool& loop)
{
    SetVol(bgmVolume_ + volumeOffset, tag);

    if (!IsPlaying(tag)) {
        Play(tag, bgmVolume_ + volumeOffset, loop);
    }
}

void Sound::PlaySE(const SoundFactory::TAG& tag, const float& volumeOffset, const bool& loop)
{
    Play(tag, seVolume_ + volumeOffset);
}

void Sound::PlayLoopSE(const SoundFactory::TAG& tag, const float& volumeOffset)
{
    if (!IsPlaying(tag)) {
        Play(tag, seVolume_ + volumeOffset, true);
    }
}

void Sound::PlayOriginSE(const SoundFactory::TAG& tag, const float& volumeOffset)
{
    if (!IsPlaying(tag)) {
        Play(tag, seVolume_ + volumeOffset, false);
    }
}

void Sound::Pause(const std::filesystem::path& path)
{
    auto it = voices_.find(path);
    if (it != voices_.end() && it->second != nullptr) {
        it->second->Stop(); // バッファは保持されたまま停止
    }
}

void Sound::Pause(const SoundFactory::TAG& tag)
{
    Pause(handleToPath_[tag]);
}

void Sound::Resume(const SoundFactory::TAG& tag) {
    Resume(handleToPath_[tag]);
}

void Sound::Resume(const std::filesystem::path& path)
{
    auto it = voices_.find(path);
    if (it != voices_.end() && it->second != nullptr) {
        it->second->Start(); // 停止した位置から再開
    }
}

void Sound::Stop(const SoundFactory::TAG& tag)
{
    Stop(handleToPath_[tag]);
}

void Sound::Stop(const std::filesystem::path& path)
{
    auto it = voices_.find(path);
    if (it != voices_.end() && it->second != nullptr) {
        it->second->Stop(); // バッファは保持されたまま停止
        it->second->Discontinuity();
        it->second->FlushSourceBuffers();
    }
}

bool Sound::IsPlaying(const SoundFactory::TAG& tag) {
    return IsPlaying(handleToPath_[tag]);
}

bool Sound::IsPlaying(const std::filesystem::path& path)
{
    auto it = voices_.find(path);
    if (it != voices_.end() && it->second != nullptr) {
        XAUDIO2_VOICE_STATE state{};
        it->second->GetState(&state);
        if (state.BuffersQueued > 0) {
            return true; // 少なくとも1つの音声が再生中
        }
    }

    return false; // 音声が停止している
}


void Sound::SetFrequencyRatio(const std::filesystem::path& path,const int ratioNum )
{
    auto it = voices_.find(path);
    if (it != voices_.end() && it->second != nullptr) {
        // XAudio2のソースボイスに比率を設定
        it->second->SetFrequencyRatio(GetRatio(ratioNum));
    }

}


void Sound::StopAllSound()
{
    for (auto& pair : voices_) {
        IXAudio2SourceVoice* voice = pair.second;
        if (voice) {
            voice->Stop();
            voice->Discontinuity();
            voice->FlushSourceBuffers();
        }
    }

}

XAUDIO2_BUFFER Sound::GetBuffer(const std::filesystem::path& path)
{
    XAUDIO2_BUFFER buf{};
    buf.pAudioData = soundDatas_[path].mediaData.data();
    buf.AudioBytes = static_cast<UINT32>(soundDatas_[path].mediaData.size());
    buf.Flags = XAUDIO2_END_OF_STREAM;

    return buf;
}

std::vector<float> Sound::GetWaveform(const SoundFactory::TAG& tag)
{
    return  GetWaveform(handleToPath_[tag]);

}

std::vector<float> Sound::GetWaveform(const std::filesystem::path& path)
{
    XAUDIO2_BUFFER buf = Sound::GetBuffer(path);
    std::vector<float> waveform;
    const int16_t* pcm = reinterpret_cast<const int16_t*>(buf.pAudioData);
    size_t sampleCount = buf.AudioBytes / sizeof(int16_t);

    // オーディオ入力コールバック側（例）
    waveform.resize(sampleCount);
    for (size_t i = 0; i < sampleCount; ++i) {
        waveform[i] = pcm[i] / 32768.0f; // -32768〜32767 → -1.0〜1.0
    }
    return waveform;
}

UINT64 Sound::GetSamplesPlayed(const SoundFactory::TAG& tag)
{
    return GetSamplesPlayed(handleToPath_[tag]);
}

UINT64 Sound::GetSamplesPlayed(const std::filesystem::path& path)
{
    XAUDIO2_VOICE_STATE state{};
    auto it = voices_.find(path);
    if (it != voices_.end() && it->second != nullptr) {
        it->second->GetState(&state);
    }

    return state.SamplesPlayed;
}

void Sound::SetFrequencyRatio(const SoundFactory::TAG& tag, const int ratioNum)
{
    SetFrequencyRatio(handleToPath_[tag],ratioNum);
}

void Sound::Unload(SoundData& soundData) {

    // メディアデータの解放
    soundData.mediaData.clear();
    soundData.pWaveFormat = {};
};

// ====================================================================================================

void Sound::Play(const SoundFactory::TAG& tag, const float& volume, const bool& isLoop) {
    Play(handleToPath_[tag], volume, isLoop);
}

void Sound::Play(const std::filesystem::path& path, const float& volume, const bool& isLoop)
{
    HRESULT result;

    IXAudio2SourceVoice* newVoice = nullptr;

    result = xAudio2_->CreateSourceVoice(&newVoice, &soundDatas_[path].pWaveFormat);
    assert(SUCCEEDED(result));
    float newVolume = volume;
    //最大値と最小値を入れる
    newVolume = std::clamp(newVolume, 0.0f, 10.0f);

    newVoice->SetVolume(newVolume);

    XAUDIO2_BUFFER buf{};
    buf.pAudioData = soundDatas_[path].mediaData.data();
    buf.AudioBytes = static_cast<UINT32>(soundDatas_[path].mediaData.size());
    buf.Flags = XAUDIO2_END_OF_STREAM;

    if (isLoop) {
        buf.LoopCount = XAUDIO2_LOOP_INFINITE;
    }

    result = newVoice->SubmitSourceBuffer(&buf);
    assert(SUCCEEDED(result));
    result = newVoice->Start();//再生開始
    assert(SUCCEEDED(result));

    voices_[path] = newVoice;

};


void Sound::SetVol(const float& vol, const SoundFactory::TAG& tag)
{
    SetVol(vol, handleToPath_[tag]);
}

void Sound::SetVol(const float& vol, const std::filesystem::path& path)
{
    auto it = voices_.find(path);
    if (it != voices_.end() && it->second != nullptr) {
        float newVolume = vol;
        //最大値と最小値を入れる
        newVolume = std::clamp(newVolume, 0.0f, 10.0f);
        it->second->SetVolume(newVolume);
    }
}

bool Sound::IsPlayingAll() {

    for (const auto& pair : voices_) {
        IXAudio2SourceVoice* voice = pair.second;
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

bool Sound::LoadFile(const std::filesystem::path& path)
{
    //読み込み済みファイルを検索
    if (soundDatas_.contains(path)) {
        LogFile::Log("Already Load Audio File!");
        return false;
    }

    //テクスチャ枚数上限チェック
    assert(soundDatas_.size() < kMaxSoundCount_);

    std::wstring filePathW = path.wstring();

    //ソースリーダーの作成
    ComPtr<IMFSourceReader> pMFSourceReader = { nullptr };
    HRESULT result = MFCreateSourceReaderFromURL(filePathW.c_str(), NULL, &pMFSourceReader);
    assert(SUCCEEDED(result));

    //メディアタイプの取得
    ComPtr<IMFMediaType> pMFMediaType{ nullptr };
    MFCreateMediaType(&pMFMediaType);
    pMFMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
    pMFMediaType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
    pMFSourceReader->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, pMFMediaType.Get());
    assert(SUCCEEDED(result));

    ComPtr<IMFMediaType> pOutType;
    pMFSourceReader->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, &pOutType);

    //オーディオデータ形式の作成　メディアタイプからWaveForMatexを生成
    WAVEFORMATEX* waveFormat{ nullptr };
    MFCreateWaveFormatExFromMFMediaType(pOutType.Get(), &waveFormat, nullptr);

    soundDatas_[path].pWaveFormat = *waveFormat;

    while (true) {
        ComPtr<IMFSample> pMFSample{ nullptr };
        DWORD streamIndex = 0, flags = 0;
        LONGLONG llTimeStamp = 0;
        //サンプルを読み込む
        result = pMFSourceReader->ReadSample(MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, &streamIndex, &flags, &llTimeStamp, &pMFSample);

        if (flags & MF_SOURCE_READERF_ENDOFSTREAM)break;

        if (pMFSample) {
            ComPtr<IMFMediaBuffer> pMFMediaBuffer{ nullptr };
            pMFSample->ConvertToContiguousBuffer(&pMFMediaBuffer);

            BYTE* pBuffer{ nullptr };
            DWORD maxLength = 0, cbCurrentLength = 0;
            pMFMediaBuffer->Lock(&pBuffer, &maxLength, &cbCurrentLength);
            soundDatas_[path].mediaData.insert(soundDatas_[path].mediaData.end(), pBuffer, pBuffer + cbCurrentLength);
            pMFMediaBuffer->Unlock();
        }

    }

    LogFile::Log("Success AudioLoad!");
    return true;
}


void Sound::LoadAndMap(const std::filesystem::path& path, const SoundFactory::TAG& tag) {

    if (LoadFile(path)) {
        //ハンドルとパスをマッピング
        handleToPath_[tag] = path;
        LogFile::Log("Mapping AudioLoad File!");
    };


}

void Sound::Load(const std::filesystem::path& path)
{
    if (LoadFile(path)) {
        //ハンドルとパスをマッピング?
        uint32_t handle = (uint32_t)handleToPath_.size();
        if (handleToPath_[handle].empty()) {
            handleToPath_[handle] = path;
        }
    
        LogFile::Log("Mapping AudioLoad File!");
    };
}

void Sound::Initialize()
{
    HRESULT result;
    result = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
    assert(SUCCEEDED(result));

    //マスターボイスの生成
    result = xAudio2_->CreateMasteringVoice(&masterVoice_);//masterVoiceはxAudio2の解放と同時に無効になるため自分でdeleteしない
    assert(SUCCEEDED(result));

    result = MFStartup(MF_VERSION, MFSTARTUP_NOSOCKET);
    assert(SUCCEEDED(result));

    soundDatas_.reserve(kMaxSoundCount_);
}

void Sound::Finalize()
{

    for (auto& pair : voices_) {
        IXAudio2SourceVoice* voice = pair.second;
        if (voice) {
            voice->Stop();
            voice->Discontinuity();
            voice->FlushSourceBuffers();
            voice->DestroyVoice();
        }
    }

    for (auto& [filePath, data] : soundDatas_) {
        Unload(data);
    }

    soundDatas_.clear();
    xAudio2_.Reset();

    HRESULT result;
    result = MFShutdown();
    assert(SUCCEEDED(result));
}
