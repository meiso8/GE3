#pragma once
class SoundFactory
{
public:
    //TAGを宣言する
    enum TAG {
        // ======================================
        //BGM
        BGM_ArabRuins,
        BGM_SandCity,
        BGM_Sea,
        BGM_Sun,
        // ======================================

        //ホラーアクセント
        HORROR1,
        HORROR2,
        //物が落ちる
        FALL,
        //水
        WATER_DROP,
        //メモ用
        BOOK,
        //スライドするとか
        MOVE_ROCK,
        //クラッカー
        CRACKER,
        //足音
        FOOT_STEP,
        //スイッチ
        SWITCH_ON,
        //正解
        CORRECT,
        //キャンセル
        BUZZER,
        //ごごごごごご
        GOGOGO,
        //ファイアボール
        FIRE_BALL,
        //うめき声
        WOO,

        // ======================================
        //ボイス
        VOICE_Asobimasyo,
        VOICE_Sottizyanaiwa,
        // ======================================
        //メジェド様の足音
        MEDJED_FOOT_STEP_SMALL,
    };

   static void Load();
};

