#include "TextureFactory.h"
#include"Texture.h"
void TextureFactory::Load()
{
    Texture::LoadAndMapHandle("Resource/Textures/white1x1.png", WHITE_1X1);
    Texture::LoadAndMapHandle("Resource/Textures/uvChecker.png", UV_CHECKER);
    Texture::LoadAndMapHandle("Resource/Textures/circle2.png", CIRCLE);
    Texture::LoadAndMapHandle("Resource/Textures/gradationLine.png", GRADATION_LINE);
    Texture::LoadAndMapHandle("Resource/Textures/noise0.png", NOIZE0);
    Texture::LoadAndMapHandle("Resource/Textures/noise1.png", NOIZE1);

    Texture::LoadAndMapHandle("Resource/Textures/numbers.png", NUMBERS);
    Texture::LoadAndMapHandle("Resource/Textures/puzzle1.png", PUZZLE1);

    Texture::LoadAndMapHandle("Resource/Textures/skip.png", SKIP);
    Texture::LoadAndMapHandle("Resource/Textures/Title.png", TITLE);
    Texture::LoadAndMapHandle("Resource/Textures/credit.png", CREDIT);
    Texture::LoadAndMapHandle("Resource/Textures/exit.png", BUTTON_EXIT);
    Texture::LoadAndMapHandle("Resource/Textures/start.png", BUTTON_START);

    Texture::LoadAndMapHandle("Resource/Textures/back.png", BUTTON_BACK_TO_GAME);
    Texture::LoadAndMapHandle("Resource/Textures/titleButton.png", BUTTON_BACK_TO_TITL);

    Texture::LoadAndMapHandle("Resource/Textures/test3.png", TEST3);

    Texture::LoadAndMapHandle("Resource/Textures/memo01.png", MEMO1);
    Texture::LoadAndMapHandle("Resource/Textures/memo02.png", MEMO2);
    Texture::LoadAndMapHandle("Resource/Textures/memo03.png", MEMO3);
    Texture::LoadAndMapHandle("Resource/Textures/memo04.png", MEMO4);
    Texture::LoadAndMapHandle("Resource/Textures/memo05.png", MEMO5);

    Texture::LoadAndMapHandle("Resource/Textures/book.png", BOOK);
    Texture::LoadAndMapHandle("Resource/Textures/book02.png", BOOK2);
    Texture::LoadAndMapHandle("Resource/Textures/book03.png", BOOK3);
    Texture::LoadAndMapHandle("Resource/Textures/book04.png", BOOK4);
    Texture::LoadAndMapHandle("Resource/Textures/book05.png", BOOK5);

    Texture::LoadAndMapHandle("Resource/Textures/none.png", NONE);
    Texture::LoadAndMapHandle("Resource/Textures/puzzle.png", PUZZLE);
    Texture::LoadAndMapHandle("Resource/Textures/puzzleNum.png", PUZZLE_NUM);

    Texture::LoadAndMapHandle("Resource/Textures/s.png", HIERO_S);
    Texture::LoadAndMapHandle("Resource/Textures/p.png", HIERO_P);
    Texture::LoadAndMapHandle("Resource/Textures/d.png", HIERO_D);
    Texture::LoadAndMapHandle("Resource/Textures/t.png", HIERO_T);

    Texture::LoadAndMapHandle("Resource/Textures/BD_Hunefer.jpg", BD_HUNEFER);

   Texture::LoadAndMapHandle("Resource/Textures/ending01.png", ENDING1);
   Texture::LoadAndMapHandle("Resource/Textures/ending02.png", ENDING2);

    Texture::LoadAndMapHandle("Resource/Textures/slot.png", SLOT);
    Texture::LoadAndMapHandle("Resource/Textures/eye.png", EYE);
    Texture::LoadAndMapHandle("Resource/Textures/world.png", WORLD);

    Texture::LoadAndMapHandle("Resource/Textures/night.dds", SKYBOX_TEX);

    Texture::LoadAndMapHandle("Resource/Textures/beam.png", BEAM);

    Texture::LoadAndMapHandle("Resource/Textures/waterTexture.png", WATER_TEXTURE);
    Texture::LoadAndMapHandle("Resource/Textures/glyph.png", GLYPH_NBT_SMT_HT);
    Texture::LoadAndMapHandle("Resource/Textures/nb.png", GLYPH_NB);
    Texture::LoadAndMapHandle("Resource/Textures/sm.png", GLYPH_SM);
    Texture::LoadAndMapHandle("Resource/Textures/h.png", GLYPH_H);
    Texture::LoadAndMapHandle("Resource/Textures/ht.png", GLYPH_HT);
    Texture::LoadAndMapHandle("Resource/Textures/n.png", GLYPH_N);

    Texture::LoadAndMapHandle("Resource/Textures/UI/UI_A.png", UI_A);
    Texture::LoadAndMapHandle("Resource/Textures/UI/UI_L.png", UI_L);
    Texture::LoadAndMapHandle("Resource/Textures/UI/UI_LB.png", UI_LB);
    Texture::LoadAndMapHandle("Resource/Textures/UI/UI_R.png", UI_R);
    Texture::LoadAndMapHandle("Resource/Textures/UI/UI_RB.png", UI_RB);
    Texture::LoadAndMapHandle("Resource/Textures/UI/UI_X.png", UI_X);

    Texture::LoadAndMapHandle("Resource/Textures/fire.png", FIRE);
}
