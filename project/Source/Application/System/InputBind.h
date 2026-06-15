#pragma once
#include"Input.h"

class InputBind
{
public:
    static bool IsClick();
    static bool IsClickPress();
    static bool IsPressMoveL();
    static bool IsPressMoveR();
    static bool IsPressMoveF();
    static bool IsPressMoveB();
    static bool IsPressSpeedButton();
    static bool IsTriggerJump();
    static bool IsClickR();
    static bool IsClickPressR();
    static bool IsTriggerMenuKey();
};

