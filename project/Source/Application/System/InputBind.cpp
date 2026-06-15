#include "InputBind.h"

bool InputBind::IsClick() {
    return (Input::IsTriggerMouse(0) || Input::IsControllerTriggerButton(XINPUT_GAMEPAD_A, 0));
}

bool InputBind::IsClickPress() {
    return (Input::IsPressMouse(0) || Input::IsControllerPressButton(XINPUT_GAMEPAD_LEFT_SHOULDER, 0));
}

bool InputBind::IsPressMoveL() {
    return (Input::IsPushKey(DIK_A));
};
bool InputBind::IsPressMoveR() {
    return (Input::IsPushKey(DIK_D));
};
bool InputBind::IsPressMoveF() {
    return (Input::IsPushKey(DIK_W));
};
bool InputBind::IsPressMoveB() {
    return (Input::IsPushKey(DIK_S));
}
bool InputBind::IsPressSpeedButton()
{
    return  (Input::IsPushKey(DIK_LSHIFT)/* || Input::IsControllerPressButton(XINPUT_GAMEPAD_LEFT_SHOULDER, 0)*/);
}
bool InputBind::IsTriggerJump()
{
    return Input::IsTriggerKey(DIK_SPACE) || Input::IsControllerTriggerButton(XINPUT_GAMEPAD_X, 0);
}
bool InputBind::IsClickR()
{
    return Input::IsTriggerMouse(1) ||Input::IsControllerPressButton(XINPUT_GAMEPAD_RIGHT_SHOULDER, 0);
}
bool InputBind::IsClickPressR()
{
    return Input::IsPressMouse(1) || Input::IsControllerPressButton(XINPUT_GAMEPAD_RIGHT_SHOULDER, 0);
}
bool InputBind::IsTriggerMenuKey()
{
    //ちょっと後でキーを確認する
    return (Input::IsTriggerKey(DIK_ESCAPE) || Input::IsControllerTriggerButton(XINPUT_GAMEPAD_START, 0));
};