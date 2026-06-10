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
        UI,
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
        // ======================
        TEXTURES,
    };

    static void Load();
};

