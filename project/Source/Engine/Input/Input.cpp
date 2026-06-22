                                                                                                                                                                                                                                                                                                      #include"Input.h"
#include <cassert>
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#include"TimeManager.h"
#include"Camera/Camera.h"
#include<cmath>
#include"Vector2.h"
#include<algorithm>
BYTE Input::key_[256];
BYTE Input::preKey_[256];


DIMOUSESTATE Input::mouseState_;
DIMOUSESTATE Input::preMouseState_;

std::array <XINPUT_STATE, 4>Input::xinputState_;
std::array <XINPUT_STATE, 4>Input::preXinputState_;
Window* Input::window_ = nullptr;

std::array <bool, 4>Input::isControllerConnected_;

bool Input::isDragging_ = false;

HRESULT Input::Initialize(Window& window/*, int& fps*/) {

    //fps_ = &fps;

    window_ = &window;

    HRESULT result;
    //DirectInputの初期化 ゲームパッドを追加するにしてもこのオブジェクトは一つでよい。
    IDirectInput8* directInput = nullptr;
    result = DirectInput8Create(
        window_->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8,
        (void**)&directInput, nullptr
    );
    assert(SUCCEEDED(result));

    //キーボードデバイスの生成
    result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard_, NULL);
    assert(SUCCEEDED(result));

    //入力データ形式のセット
    result = keyboard_->SetDataFormat(&c_dfDIKeyboard);//標準形式 キーボードの場合
    assert(SUCCEEDED(result));

    //排他制御レベルのセット
    result = keyboard_->SetCooperativeLevel(
        window_->GetHwnd(),
        DISCL_FOREGROUND//画面が手前にある場合のみ入力を受け付ける
        | DISCL_NONEXCLUSIVE //デバイスをこのアプリだけで占有しない
        | DISCL_NOWINKEY//Windowsキーを無効にする
    );
    assert(SUCCEEDED(result));

    //マウスデバイスの生成
    result = directInput->CreateDevice(GUID_SysMouse, &mouse_, NULL);
    assert(SUCCEEDED(result));

    // マウス用のデータ形式のセット
    result = mouse_->SetDataFormat(&c_dfDIMouse);
    assert(SUCCEEDED(result));

    // モードを設定（フォアグラウンド＆非排他モード）
    result = mouse_->SetCooperativeLevel(
        window_->GetHwnd(),
        DISCL_FOREGROUND//画面が手前にある場合のみ入力を受け付ける
        | DISCL_NONEXCLUSIVE //デバイスをこのアプリだけで占有しない
        | DISCL_NOWINKEY//Windowsキーを無効にする
    );
    assert(SUCCEEDED(result));

    // デバイスの設定
    DIPROPDWORD diprop;
    diprop.diph.dwSize = sizeof(diprop);
    diprop.diph.dwHeaderSize = sizeof(diprop.diph);
    diprop.diph.dwObj = 0;
    diprop.diph.dwHow = DIPH_DEVICE;
    diprop.dwData = DIPROPAXISMODE_REL;	// 相対値モードで設定（絶対値はDIPROPAXISMODE_ABS）

    for (int i = 0; i < 4; ++i) {
        isControllerConnected_[i] = false;
    }

    result = mouse_->SetProperty(DIPROP_AXISMODE, &diprop.diph);
    assert(SUCCEEDED(result));
    return result;
};

bool Input::IsPressKey(const uint8_t& keyNum) {

    if (key_[keyNum]) {
        return true;
    }

    return false;

}
bool Input::IsTriggerKey(const uint8_t& keyNum) {

    if (key_[keyNum] && !preKey_[keyNum]) {
        return true;
    }

    return false;

}
bool Input::IsReleaseStateKey(const uint8_t& keyNum) {

    if (!key_[keyNum]) {
        return true;
    }

    return false;

}
bool Input::IsReleaseKey(const uint8_t& keyNum) {

    if (!key_[keyNum] && preKey_[keyNum]) {
        return true;
    }

    return false;

}

bool Input::IsAnyKeyPressed() {

    for (int i = 0; i < 256; ++i) {
        if (key_[i]) {
            return true;
        }
    }
    return false;
}


void Input::Update() {

    //キーの状態をコピーする
    memcpy(preKey_, key_, 256);
    //キーボード情報の取得開始
    keyboard_->Acquire();
    //全キー入力状態を取得する
    keyboard_->GetDeviceState(sizeof(key_), key_);

    //マウスの状態をコピーする
    memcpy(&preMouseState_, &mouseState_, sizeof(mouseState_));
    // 入力制御開始
    mouse_->Acquire();
    //マウスの状態を取得する
    mouse_->GetDeviceState(sizeof(DIMOUSESTATE), &mouseState_);


    for (int i = 0; i < 4; ++i) {
        if (isControllerConnected_[i]) {
            memcpy(&preXinputState_[i], &xinputState_[i], sizeof(XINPUT_STATE));
        }
    }

    for (int i = 0; i < 4; ++i) {
        isControllerConnected_[i] = IsControllerConnected(i) ? true : false;
    }


}

bool Input::IsControllerStickPosMove(ButtonType index, DWORD dwUserIndex, Vector2* pos)
{
    if (isControllerConnected_[dwUserIndex])
    {
        if (index == ButtonType::BUTTON_LEFT) {
            SHORT lx = xinputState_[dwUserIndex].Gamepad.sThumbLX; // 左スティックX軸
            SHORT ly = xinputState_[dwUserIndex].Gamepad.sThumbLY; // 左スティックY軸
            if (IsControllerStickMove(lx, ly))
            {
                *pos = NormalizeButtonCount(xinputState_[dwUserIndex].Gamepad.sThumbLX, xinputState_[dwUserIndex].Gamepad.sThumbLY);
                return true;
            }
        } else if (index == ButtonType::BUTTON_RIGHT) {
            SHORT rx = xinputState_[dwUserIndex].Gamepad.sThumbRX; // 右スティックX軸
            SHORT ry = xinputState_[dwUserIndex].Gamepad.sThumbRY; // 右スティックY軸
            if (IsControllerStickMove(rx, ry))
            {
                *pos = NormalizeButtonCount(xinputState_[dwUserIndex].Gamepad.sThumbRX, xinputState_[dwUserIndex].Gamepad.sThumbRY);
                return true;
            }
        }
    }

    *pos = { 0.0f,0.0f };
    return false;
}
bool Input::IsControllerStickPosMoveTrigger(ButtonType index, DWORD dwUserIndex, Vector2* pos)
{
    if (isControllerConnected_[dwUserIndex])
    {
        if (index == ButtonType::BUTTON_LEFT) {
            SHORT lx = xinputState_[dwUserIndex].Gamepad.sThumbLX; // 左スティックX軸
            SHORT ly = xinputState_[dwUserIndex].Gamepad.sThumbLY; // 左スティックY軸

            SHORT preLx = preXinputState_[dwUserIndex].Gamepad.sThumbLX; // 右スティックX軸
            SHORT preLy = preXinputState_[dwUserIndex].Gamepad.sThumbLY; // 右スティックY軸

            if (IsControllerStickMove(lx, ly) && !IsControllerStickMove(preLx, preLy))
            {
                *pos = NormalizeButtonCount(lx, ly);
                return true;
            }
        } else if (index == ButtonType::BUTTON_RIGHT) {

            SHORT rx = xinputState_[dwUserIndex].Gamepad.sThumbRX; // 右スティックX軸
            SHORT ry = xinputState_[dwUserIndex].Gamepad.sThumbRY; // 右スティックY軸

            SHORT preRx = preXinputState_[dwUserIndex].Gamepad.sThumbRX; // 右スティックX軸
            SHORT preRy = preXinputState_[dwUserIndex].Gamepad.sThumbRY; // 右スティックY軸

            if (IsControllerStickMove(rx, ry) && !IsControllerStickMove(preRx, preRy))
            {
                *pos = NormalizeButtonCount(rx, ry);
                return true;
            }
        }
    }

    *pos = { 0.0f,0.0f };
    return false;
}
bool Input::IsControllerStickMove(SHORT& buttonX, SHORT& buttonY)
{
    return abs(buttonX) > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE || abs(buttonY) > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
}
Vector2 Input::NormalizeButtonCount(SHORT& buttonX, SHORT& buttonY)
{

    float normX = (static_cast<float>(buttonX) / SHRT_MAX);
    float normY = (static_cast<float>(buttonY) / SHRT_MAX);

    normX = std::clamp(normX, -1.0f, 1.0f);
    normY = std::clamp(normY, -1.0f, 1.0f);

    //if (absX == 1.0f || absY < deadZone_) {
    //    normY = 0.0f;
    //}

    //if (absY == 1.0f || absX < deadZone_) {
    //    normX = 0.0f;
    //}

    return  { normX,normY };

}

bool Input::IsControllerConnected(DWORD dwUserIndex)
{
    ZeroMemory(&xinputState_[dwUserIndex], sizeof(XINPUT_STATE));
    return (XInputGetState(dwUserIndex, &xinputState_[dwUserIndex]) == ERROR_SUCCESS);
}

//XINPUT_STATE& Input::GetControllerState(DWORD& dwUserIndex)
//{
//
//    return xinputState_[dwUserIndex];
//}

void Input::VibrateController(DWORD dwUserIndex, WORD leftMotor, WORD rightMotor)
{
    if (isControllerConnected_[dwUserIndex]) {
        XINPUT_VIBRATION vibration;
        vibration.wLeftMotorSpeed = leftMotor;   // 0 ～ 65535（低周波）
        vibration.wRightMotorSpeed = rightMotor; // 0 ～ 65535（高周波）
        XInputSetState(dwUserIndex, &vibration);
    }
}



bool Input::IsControllerPressButton(UINT16 button, DWORD dwUserIndex)
{
    if (isControllerConnected_[dwUserIndex])
    {
        if (xinputState_[dwUserIndex].Gamepad.wButtons & button) {
            return true;
        }
    }

    return false;
}

bool Input::IsControllerTriggerButton(UINT16 button, DWORD dwUserIndex)
{
    if (isControllerConnected_[dwUserIndex])
    {
        if (!(preXinputState_[dwUserIndex].Gamepad.wButtons & button) && (xinputState_[dwUserIndex].Gamepad.wButtons & button)) {
            return true;
        }
    }

    return false;
}


bool Input::IsControllerPressLTRT(ButtonType index, DWORD dwUserIndex)
{
    if (isControllerConnected_[dwUserIndex]) {
        if (index == 0) {
            return IsControllerDeadZone(xinputState_[dwUserIndex].Gamepad.bLeftTrigger);
        } else {
            return IsControllerDeadZone(xinputState_[dwUserIndex].Gamepad.bRightTrigger);
        }
    }

    return false;
};


bool Input::IsControllerTriggerLTRT(ButtonType index, DWORD dwUserIndex)
{
    if (isControllerConnected_[dwUserIndex])
    {
        if (index == 0) {
            return (IsControllerDeadZone(xinputState_[dwUserIndex].Gamepad.bLeftTrigger) && !IsControllerDeadZone(preXinputState_[dwUserIndex].Gamepad.bLeftTrigger));
        } else {
            return (IsControllerDeadZone(xinputState_[dwUserIndex].Gamepad.bRightTrigger) && !IsControllerDeadZone(preXinputState_[dwUserIndex].Gamepad.bRightTrigger));
        }
    }

    return false;
}

BYTE Input::GetControllerTriggerCount(ButtonType index, DWORD dwUserIndex)
{
    if (isControllerConnected_[dwUserIndex])
    {
        if (index == 0) {
            if (IsControllerDeadZone(xinputState_[dwUserIndex].Gamepad.bLeftTrigger))
            {
                return xinputState_[dwUserIndex].Gamepad.bLeftTrigger;
            }
        } else {
            if (IsControllerDeadZone(xinputState_[dwUserIndex].Gamepad.bRightTrigger))
            {
                return xinputState_[dwUserIndex].Gamepad.bRightTrigger;
            }
        }
    }

    return 0;
}

bool Input::IsControllerDeadZone(BYTE& triggerButton)
{
    return triggerButton > XINPUT_GAMEPAD_TRIGGER_THRESHOLD;
}

Vector2& Input::GetCursorPosition()
{
    static Vector2 cursorPos; // 静的変数を使用して左辺値を確保  
    POINT pos;
    GetCursorPos(&pos);
    ScreenToClient(window_->GetHwnd(), &pos);
    cursorPos = { static_cast<float>(pos.x), static_cast<float>(pos.y) };
    return cursorPos;
}

Vector2Int Input::GetCursorPositionInt()
{
    POINT pos;
    GetCursorPos(&pos);
    ScreenToClient(window_->GetHwnd(), &pos);
    return { pos.x,pos.y };
}

void Input::SetCursorPosition(const Vector2& pos)
{
    SetCursorPos(static_cast<int>(pos.x), static_cast<int>(pos.y));
}

bool Input::IsPressMouse(uint32_t index) {
    assert(index < 4);
    return (mouseState_.rgbButtons[index] & 0x80) ? true : false;
}

bool Input::IsTriggerMouse(uint32_t index) {
    assert(index < 4);

    if (mouseState_.rgbButtons[index] & 0x80 && !(preMouseState_.rgbButtons[index] & 0x80)) {
        return true;
    }

    return false;
}

Vector2& Input::GetMousePos() {
    static Vector2 mousePos; // 静的変数を使用して左辺値を確保  
    mousePos.x = static_cast<float>(mouseState_.lX);
    mousePos.y = static_cast<float>(mouseState_.lY);
    return mousePos;
}

Vector2& Input::GetMousePosFiltered()
{
    static Vector2 mousePos; // 静的変数を使用して左辺値を確保 
    mousePos.x = static_cast<float>(mouseState_.lX);
    mousePos.y = static_cast<float>(mouseState_.lY);
    mousePos.x = (std::abs(mousePos.x) < 1.0f) ? 0.0f : mousePos.x;
    mousePos.y = (std::abs(mousePos.y) < 1.0f) ? 0.0f : mousePos.y;
    return mousePos;

}

float Input::GetMouseWheel() {
    return  static_cast<float>(mouseState_.lZ) * Time::DeltaTime();
}



Input::~Input() {

    mouse_->Unacquire();
    mouse_->Release();
    mouse_ = nullptr;
}

void Input::SetShowCursor(const bool flag)
{
    ShowCursor(flag);
}
