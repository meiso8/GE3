#pragma once
#include"../Engine/FreeTypeManager/Text.h"

class PasswordText
{
public:
    void Initialize();
    void Update();
    void Draw();
private:
    std::u32string inputText_;
    Text text_;
    uint32_t handle_;

};

