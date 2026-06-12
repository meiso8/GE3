#include "SoundFactory.h"
#include"Sound.h"

void SoundFactory::Load()
{
    // ======================================
    Sound::Load("Resource/Sounds/BGM/ArabRuins.mp3", BGM_ArabRuins);
    Sound::Load("Resource/Sounds/BGM/SandCity.mp3", BGM_SandCity);
    Sound::Load("Resource/Sounds/BGM/sea.mp3", BGM_Sea);
    Sound::Load("Resource/Sounds/BGM/Sun.mp3", BGM_Sun);
    // ======================================

    Sound::Load("Resource/Sounds/externals/Horror_Accent02-1(Far).mp3", HORROR1);
    Sound::Load("Resource/Sounds/externals/Horror_Accent16-2(Mid-Long).mp3", HORROR2);
    Sound::Load("Resource/Sounds/externals/maou_se_sound_fall02.mp3", FALL);
    Sound::Load("Resource/Sounds/externals/Water_Drop02-1(Low-Reverb).mp3", WATER_DROP);
    Sound::Load("Resource/Sounds/externals/Book01-1(Flip).mp3", BOOK);
    Sound::Load("Resource/Sounds/moveRock.mp3", MOVE_ROCK);
    Sound::Load("Resource/Sounds/cracker_short.mp3", CRACKER);
    Sound::Load("Resource/Sounds/externals/maou_se_sound_footstep02.mp3", FOOT_STEP);
    Sound::Load("Resource/Sounds/externals/maou_se_sound_switch01.mp3", SWITCH_ON);
    Sound::Load("Resource/Sounds/correct.mp3", CORRECT);
    Sound::Load("Resource/Sounds/externals/Quiz-Buzzer03-2(Short).mp3", BUZZER);
    Sound::Load("Resource/Sounds/externals/Onoma-Rumble02-2(Hard-Short).mp3", GOGOGO);
    Sound::Load("Resource/Sounds/externals/fireBall.mp3", FIRE_BALL);
    Sound::Load("Resource/Sounds/externals/woo.mp3", WOO);

    // ======================================
    Sound::Load("Resource/Sounds/externals/asobimasyo.mp3", VOICE_Asobimasyo);
    Sound::Load("Resource/Sounds/externals/sottizyanaiwa.mp3", VOICE_Sottizyanaiwa);
    // ======================================

    Sound::Load("Resource/Sounds/externals/medjedFootStep_Small.mp3", MEDJED_FOOT_STEP_SMALL);
    Sound::Load("Resource/Sounds/externals/beam.mp3", BEAM);
    Sound::Load("Resource/Sounds/jump.mp3", MEDJED_JUMP);
}
