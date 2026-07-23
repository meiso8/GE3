#pragma once
class TextureFactory
{
public:
    enum Handle {
        WHITE_1X1,
        UV_CHECKER,
        CIRCLE,
        GRADATION_LINE,
        NOIZE0,
        NOIZE1,
    
        NUMBERS,
        
        PUZZLE1,

        SKIP,
        TITLE,
        CREDIT,
        BUTTON_EXIT,
        BUTTON_START,


        BUTTON_BACK_TO_GAME,
        BUTTON_BACK_TO_TITL,

        TEST3,

        MEMO1,
        MEMO2,
        MEMO3,
        MEMO4,
        MEMO5,

        GLYPH_NB,
        GLYPH_SM,
        GLYPH_HT,
        GLYPH_H,

        BOOK,
        BOOK2,
        BOOK3,
        BOOK4,
        NONE,
        PUZZLE,
        PUZZLE_NUM,

        HIERO_S,
        HIERO_P,
        HIERO_D,
        HIERO_T,

        BD_HUNEFER,

        ENDING1,
        ENDING2,

        SLOT,
        EYE,

        WORLD,
        SKYBOX_TEX,
        BEAM,
        WATER_TEXTURE,

        FIRE,
        //一旦この順序出ないとバグになるね
        BOOK5,
        //N　波
        GLYPH_N,

        UI_A,
        UI_L,
        UI_LB,
        UI_R,
        UI_RB,
        UI_X,

    
        ARROW,
        FOOTS_PRINTS,
        MELT_GLASS,
        NORMAL_GLASS,
        MUSIC,
        //後でここら辺のシステムを変更する。6番
        BOOK6,
        //手
        HAND,
        MUSIC_HINT,
        // ======================
        TEXTURES,
    };

    static void Load();
};

