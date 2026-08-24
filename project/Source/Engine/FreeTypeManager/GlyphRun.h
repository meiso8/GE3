#pragma once
//freeTypeを使用するためのインクルードヘッダ
#include <ft2build.h>
//FT_FREETYPE_H はマクロです。
#include FT_FREETYPE_H

#include"Vector2.h"

struct GlyphRun {
    //文字の添え字
    FT_UInt glyphIndex;
    //位置
    Vector2 position;
};
