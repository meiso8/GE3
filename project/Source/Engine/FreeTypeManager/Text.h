#pragma once
#include <string>
#include <vector>
#include "Vector2.h"
#include "Vector4.h"
#include "FreeTypeManager.h"
#include"Engine/Mesh/Font/Font.h"
enum class TextAlign {
    Left,
    Center,
    Right
};
enum class VerticalAlign { 
    Top,
    Middle,
    Bottom
};

class Text {
public:
    void Initialize(uint32_t fontHandle);
    void SetString(const std::u32string& text);
    void SetPosition(const Vector2& pos);
    void SetColor(const Vector4& color);
    void SetAlign(TextAlign align);
    void SetVerticalAlign(VerticalAlign align) { verticalAlign_ = align; }
    void SetBlendMode(const BlendMode& blendMode);
    void SetSize(const Vector2 size) { size_ = size; }
    void Draw();
    void Debug();

private:
    void UpdateLayout();

private:
    uint32_t fontHandle_ = 0;
    std::u32string text_;
    Vector2 position_ = { 0.0f, 0.0f };
    Vector4 color_ = { 1.0f, 1.0f, 1.0f, 1.0f };
    TextAlign align_ = TextAlign::Left;
    BlendMode blendMode_ = kBlendModeNone;
    Vector2 size_ = { 64.0f,64.0f };
    std::vector<GlyphRun> glyphRuns_;
    std::vector<Font*> activeFonts_;
    VerticalAlign verticalAlign_ = VerticalAlign::Middle;

};
