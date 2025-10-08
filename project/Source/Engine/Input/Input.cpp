#include"Input.h"
#include <cassert>
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

#include"Camera/Camera.h"
#include"math/Normalize.h"

Input* Input::instance_ = nullptr;

Input* Input::GetInstance() {

    if (instance_ == nullptr) {
        instance_ = new Input();
    }
    return instance_;
}

BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext) {
    auto* self = static_cast<Input*>(pContext);
    self->joystickGUID = pdidInstance->guidInstance;
    self->foundJoystick_ = true;
    return DIENUM_STOP; // 最初のデバイスだけ取得
}

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

    result = mouse_->SetProperty(DIPROP_AXISMODE, &diprop.diph);
    assert(SUCCEEDED(result));


    directInput->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback, this, DIEDFL_ATTACHEDONLY);

    if (foundJoystick_) {
        HRESULT result = directInput->CreateDevice(GUID_Joystick, &gamePad_, NULL);
        assert(SUCCEEDED(result));

        //入力データ形式のセット
        result = gamePad_->SetDataFormat(&c_dfDIJoystick);//標準形式 
        assert(SUCCEEDED(result));

        //排他制御レベルのセット
        result = gamePad_->SetCooperativeLevel(
            window_->GetHwnd(),
            DISCL_FOREGROUND//画面が手前にある場合のみ入力を受け付ける
            | DISCL_NONEXCLUSIVE //デバイスをこのアプリだけで占有しない
            | DISCL_NOWINKEY//Windowsキーを無効にする
        );
        assert(SUCCEEDED(result));
    }

    return result;

};

bool Input::IsPushKey(const uint8_t& keyNum) {

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

void Input::Update() {

    //キーの状態をコピーする
    memcpy(preKey_, key_, 256);
    //キーボード情報の取得開始
    keyboard_->Acquire();
    //全キー入力状態を取得する
    keyboard_->GetDeviceState(sizeof(key_), key_);

    //マウスの状態をコピーする
    memcpy(&mouseState_bak_, &mouseState_, sizeof(mouseState_bak_));
    // 入力制御開始
    mouse_->Acquire();
    //マウスの状態を取得する
    mouse_->GetDeviceState(sizeof(DIMOUSESTATE), &mouseState_);


    if (foundJoystick_) {
        ////ゲームパッドの状態を取得する
        memcpy(preJoyButtons_, joyState_.rgbButtons, 32);
        gamePad_->Acquire();
        gamePad_->Poll(); // デバイスにポーリング
        gamePad_->GetDeviceState(sizeof(DIJOYSTATE), &joyState_);
    }
}


bool Input::GetJoyStickPos(float* x, float* y, ButtonType buttonType) {

    if (foundJoystick_) {

        if (!x || !y) {
            return 0;
        }

        if (buttonType == BUTTON_LEFT) {
            return NormalizeButtonCount(x, y, joyState_.lX, joyState_.lY);
        }

        if (buttonType == BUTTON_RIGHT) {
            return NormalizeButtonCount(x, y, joyState_.lRx, joyState_.lRy);
        }

    }

    *x = 0.0f;
    *y = 0.0f;
    return 0;
}

bool Input::GetJoyStickDPadButton(float* x, float* y)
{

    if (!foundJoystick_) {
        return false;
    }
    if (joyState_.rgdwPOV[0] == -1) {
        *x = 0.0f;
        *y = 0.0f;
        return false;
    }

    if (joyState_.rgdwPOV[0] == 0) {
        //上
        *x = 0.0f;
        *y = 1.0f;
    }

    if (joyState_.rgdwPOV[0] == 4500) {
        //上
        *x = 0.707107f;
        *y = 0.707107f;
    }

    if (joyState_.rgdwPOV[0] == 9000) {
        *x = 1.0f;
        *y = 0.0f;
    }

    if (joyState_.rgdwPOV[0] == 13500) {
        //上
        *x = 0.707107f;
        *y = -0.707107f;
    }

    if (joyState_.rgdwPOV[0] == 18000) {
        //上
        *x = 0.0f;
        *y = -1.0f;
    }
    if (joyState_.rgdwPOV[0] == 22500) {
        *x = -0.707107f;
        *y = -0.707107f;
    }

    if (joyState_.rgdwPOV[0] == 27000) {
        //左
        *x = -1.0f;
        *y = 0.0f;
    }

    if (joyState_.rgdwPOV[0] == 31500) {
        //左上
        //*x = -1.0f / std::sqrtf(2.0f);
        *x =-0.707107f;
        *y = 0.707107f;
    }

    return false;
}

bool Input::NormalizeButtonCount(float* x, float* y, LONG& buttonLX, LONG& buttonLY)
{
    float normX = (static_cast<float>(buttonLX) - SHRT_MAX) / SHRT_MAX;
    float normY = (static_cast<float>(buttonLY) - SHRT_MAX) / SHRT_MAX;

    if (normX > 1.0f - deadZone_) {
        normX = 1.0f;
    } else if (normX < -1.0f + deadZone_) {
        normX = -1.0f;
    }

    if (normY > 1.0f - deadZone_) {
        normY = 1.0f;
    } else if (normY < -1.0f + deadZone_) {
        normY = -1.0f;
    }

    float absX = std::fabsf(normX);
    float absY = std::fabsf(normY);

    if (absX == 1.0f || absY < deadZone_) {
        normY = 0.0f;
    }

    if (absY == 1.0f || absX < deadZone_) {
        normX = 0.0f;
    }

    *x = normX;
    *y = -normY;

    return 1;

}



bool Input::IsJoyStickPressButton(uint32_t index) {
    if (foundJoystick_) {

        if (index > 31) {
            return false;
        }

        if (joyState_.rgbButtons[index] & 0x80) {
            return true;
        }


    }
    return false;
}
bool Input::IsJoyStickTrigger(uint32_t index)
{
    if (foundJoystick_) {

        if (index > 31) {
            return false;
        }
        if (joyState_.rgbButtons[index] & 0x80 && !(preJoyButtons_[index] & 0x80)) {
            return true;
        }
    }
    return false;
}
;

bool Input::IsPressMouse(uint32_t index) {
    return (mouseState_.rgbButtons[index] & 0x80) ? true : false;
}

Vector2& Input::GetMousePos() {
    static Vector2 mousePos; // 静的変数を使用して左辺値を確保  
    mousePos.x = static_cast<float>(mouseState_.lX);
    mousePos.y = static_cast<float>(mouseState_.lY);
    return mousePos;
}

float Input::GetMouseWheel() {
    mouseWheelVol_ += static_cast<float>(mouseState_.lZ) / FPS;
    return mouseWheelVol_;
};

void Input::EyeOperation(Camera& camera) {

    if (IsPressMouse(2) && IsPushKey(DIK_LSHIFT)) {
        //視点の移動 offset をずらす
        //後でoffsetをくわえる
        offset_ += GetMousePos();
        camera.SetOffset({ offset_.x / FPS,offset_.y / FPS * 2.0f });
    } else if (IsPressMouse(2)) {
        //視点の回転
        //中ボタン押し込み&&ドラッグ
        isDragging_ = true;
    }

    //マウススクロールする //初期位置-10
    shericalCoordinate_.radius = -30 + GetMouseWheel();

    if (!IsPressMouse(2)) {
        isDragging_ = false;
    }

    if (isDragging_) {
        currentPos_ = GetMousePos();
        shericalCoordinate_.polar += currentPos_.x / FPS;
        shericalCoordinate_.azimuthal += currentPos_.y / FPS;
        camera.SetRotateY(shericalCoordinate_.polar);
        camera.SetRotateZ(shericalCoordinate_.azimuthal);
    }

    pos_ = TransformCoordinate(shericalCoordinate_);

    camera.SetTranslate(pos_);

}

Input::~Input() {

    mouse_->Unacquire();
    mouse_->Release();
    mouse_ = nullptr;

}