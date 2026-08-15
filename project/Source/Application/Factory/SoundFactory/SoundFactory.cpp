#include "SoundFactory.h"
#include"Sound.h"

void SoundFactory::Load()
{
    // ======================================
    Sound::LoadAndMap("Resource/Sounds/BGM/ArabRuins.mp3", BGM_ArabRuins);
    Sound::LoadAndMap("Resource/Sounds/BGM/SandCity.mp3", BGM_SandCity);
    Sound::LoadAndMap("Resource/Sounds/BGM/sea.mp3", BGM_Sea);
    Sound::LoadAndMap("Resource/Sounds/BGM/Sun.mp3", BGM_Sun);
    // ======================================

    Sound::LoadAndMap("Resource/Sounds/externals/Horror_Accent02-1(Far).mp3", HORROR1);
    Sound::LoadAndMap("Resource/Sounds/externals/Horror_Accent16-2(Mid-Long).mp3", HORROR2);
    Sound::LoadAndMap("Resource/Sounds/externals/maou_se_sound_fall02.mp3", FALL);
    Sound::LoadAndMap("Resource/Sounds/externals/Water_Drop02-1(Low-Reverb).mp3", WATER_DROP);
    Sound::LoadAndMap("Resource/Sounds/externals/Book01-1(Flip).mp3", BOOK);
    Sound::LoadAndMap("Resource/Sounds/moveRock.mp3", MOVE_ROCK);
    Sound::LoadAndMap("Resource/Sounds/cracker_short.mp3", CRACKER);
    Sound::LoadAndMap("Resource/Sounds/externals/maou_se_sound_footstep02.mp3", FOOT_STEP);
    Sound::LoadAndMap("Resource/Sounds/externals/maou_se_sound_switch01.mp3", SWITCH_ON);
    Sound::LoadAndMap("Resource/Sounds/correct.mp3", CORRECT);
    Sound::LoadAndMap("Resource/Sounds/externals/Quiz-Buzzer03-2(Short).mp3", BUZZER);
    Sound::LoadAndMap("Resource/Sounds/externals/Onoma-Rumble02-2(Hard-Short).mp3", GOGOGO);
    Sound::LoadAndMap("Resource/Sounds/externals/fireBall.mp3", FIRE_BALL);
    Sound::LoadAndMap("Resource/Sounds/externals/woo.mp3", WOO);

    // ======================================
    Sound::LoadAndMap("Resource/Sounds/externals/asobimasyo.mp3", VOICE_Asobimasyo);
    Sound::LoadAndMap("Resource/Sounds/externals/sottizyanaiwa.mp3", VOICE_Sottizyanaiwa);
    // ======================================

    Sound::LoadAndMap("Resource/Sounds/externals/medjedFootStep_Small.mp3", MEDJED_FOOT_STEP_SMALL);
    Sound::LoadAndMap("Resource/Sounds/externals/beam.mp3", BEAM);
    Sound::LoadAndMap("Resource/Sounds/jump.mp3", MEDJED_JUMP);

    Sound::LoadAndMap("Resource/Sounds/externals/maou_se_inst_piano1_1do.mp3", Sound_C);
    Sound::LoadAndMap("Resource/Sounds/externals/fire.mp3", Fire);

    Sound::LoadAndMap("Resource/Sounds/thermography.mp3", Thermography);


    Sound::LoadAndMap("Resource/Sounds/lookFor.mp3", LookFor);

    Sound::LoadAndMap("Resource/Sounds/piano.mp3", Piano);
}
