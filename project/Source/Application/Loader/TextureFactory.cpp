#include "TextureFactory.h"
#include"Texture.h"
void TextureFactory::Load()
{
    Texture::Load("Resource/Textures/white1x1.png", WHITE_1X1);
    Texture::Load("Resource/Textures/uvChecker.png", UV_CHECKER);
    Texture::Load("Resource/Textures/circle2.png", CIRCLE);
    Texture::Load("Resource/Textures/gradationLine.png", GRADATION_LINE);
    Texture::Load("Resource/Textures/noise0.png", NOIZE0);
    Texture::Load("Resource/Textures/noise1.png", NOIZE1);

    Texture::Load("Resource/Textures/numbers.png", NUMBERS);
    Texture::Load("Resource/Textures/ui.png", UI);

    Texture::Load("Resource/Textures/skip.png", SKIP);
    Texture::Load("Resource/Textures/Title.png", TITLE);
    Texture::Load("Resource/Textures/credit.png", CREDIT);
    Texture::Load("Resource/Textures/exit.png", BUTTON_EXIT);
    Texture::Load("Resource/Textures/start.png", BUTTON_START);

    Texture::Load("Resource/Textures/back.png", BUTTON_BACK_TO_GAME);
    Texture::Load("Resource/Textures/titleButton.png", BUTTON_BACK_TO_TITL);

    Texture::Load("Resource/Textures/test3.png", TEST3);

    Texture::Load("Resource/Textures/memo01.png", MEMO1);
    Texture::Load("Resource/Textures/memo02.png", MEMO2);
    Texture::Load("Resource/Textures/memo03.png", MEMO3);
    Texture::Load("Resource/Textures/memo04.png", MEMO4);
    Texture::Load("Resource/Textures/memo05.png", MEMO5);

    Texture::Load("Resource/Textures/book.png", BOOK);
    Texture::Load("Resource/Textures/book02.png", BOOK2);
    Texture::Load("Resource/Textures/book03.png", BOOK3);
    Texture::Load("Resource/Textures/book04.png", BOOK4);

    Texture::Load("Resource/Textures/none.png", NONE);
    Texture::Load("Resource/Textures/puzzle.png", PUZZLE);
    Texture::Load("Resource/Textures/puzzleNum.png", PUZZLE_NUM);

    Texture::Load("Resource/Textures/s.png", HIERO_S);
    Texture::Load("Resource/Textures/p.png", HIERO_P);
    Texture::Load("Resource/Textures/d.png", HIERO_D);
    Texture::Load("Resource/Textures/t.png", HIERO_T);
    Texture::Load("Resource/Textures/BD_Hunefer.jpg", BD_HUNEFER);

   Texture::Load("Resource/Textures/ending01.png", ENDING1);
   Texture::Load("Resource/Textures/ending02.png", ENDING2);

    Texture::Load("Resource/Textures/slot.png", SLOT);
    Texture::Load("Resource/Textures/eye.png", EYE);
    Texture::Load("Resource/Textures/world.png", WORLD);

    Texture::Load("Resource/Textures/night.dds", SKYBOX_TEX);

    Texture::Load("Resource/Textures/beam.png", BEAM);
}
