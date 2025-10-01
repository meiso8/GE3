#include"Input.h"
#include <cassert>
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

#include"Camera/Camera.h"
#include"math/Normalize.h"

Input* Input::instance_ = nullptr;

BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext) {
    auto* self = static_cast<Input*>(pContext);
    self->joystickGUID = pdidInstance->guidInstance;
    self->foundJoystick_ = true;
    return DIENUM_STOP; // 最初のデバイスだけ取得
}

HRESULT Input::Initialize(Window& window, int& fps) {

    fps_ = &fps;

    HRESULT result;
    //DirectInputの初期化 ゲームパッドを追加するにしてもこのオブジェクトは一つでよい。
    IDirectInput8* directInput = nullptr;
    result = DirectInput8Create(
        window.GetWindowClass().hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
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
        window.GetHwnd(),
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
        window.GetHwnd(),
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
            window.GetHwnd(),
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
        gamePad_->Acquire();
        gamePad_->Poll(); // デバイスにポーリング
        gamePad_->GetDeviceState(sizeof(DIJOYSTATE), &joyState_);
    }
}


bool Input::GetJoyStick(int stickNo, float* x, float* y) {
    if (foundJoystick_) {
        if (!x || !y) {
            return 0;
        }

        if (stickNo == 0) {

            if (joyState_.lX >= SHRT_MAX - deadZone_ && joyState_.lX <= SHRT_MAX + deadZone_) {
                return 0;
            }

            if (joyState_.lY >= SHRT_MAX - deadZone_ && joyState_.lY <= SHRT_MAX + deadZone_) {
                return 0;
            }

            Vector2 normL = Normalize(Vector2(static_cast<float>(joyState_.lX), static_cast<float>(joyState_.lY)));

            if (joyState_.lX >= 0.0f && joyState_.lX <= SHRT_MAX - deadZone_) {
                *x = -normL.x;
            } else if (joyState_.lX >= SHRT_MAX + deadZone_) {
                *x = normL.x;
            } else {
                *x = 0.0f;
            }

            if (joyState_.lY >= 0.0f && joyState_.lY <= SHRT_MAX - deadZone_) {
                *y = -normL.y;
            } else if (joyState_.lY >= SHRT_MAX + deadZone_) {
                *y = normL.y;
            } else {
                *y = 0.0f;
            }


            return 1;
        }

    }


    *x = 0.0f;
    *y = 0.0f;
    return 0;
}



bool Input::IsJoyStickPressButton(uint32_t index) {
    if (foundJoystick_) {
        if (joyState_.rgbButtons[index] & 0x80) {
            return true;
        }
    }
    return false;
};


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
    mouseWheelVol_ += static_cast<float>(mouseState_.lZ) / *fps_;
    return mouseWheelVol_;
};

void Input::EyeOperation(Camera& camera) {

    if (IsPressMouse(2) && IsPushKey(DIK_LSHIFT)) {
        //視点の移動 offset をずらす
        //後でoffsetをくわえる
        offset_ += GetMousePos();
        camera.SetOffset({ offset_.x / *fps_ ,offset_.y / *fps_ * 2.0f });
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
        shericalCoordinate_.polar += currentPos_.x / *fps_;
        shericalCoordinate_.azimuthal += currentPos_.y / *fps_;
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