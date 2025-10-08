#pragma once
#define DIRECTINPUT_VERSION 0x0800//DirectXバージョン指定
//　DIRECTINPUT_VERSION　dinput.hのインクルードより上に書くこと。
#include <dinput.h>
#include<cstdint>
#include"Vector2.h"
#include"Window.h"
#include"SphericalCoordinate.h"
#include"Window.h"
#include<memory>
#define FPS 60.0f

class Camera;

class Input {
public:
    static bool foundJoystick_;
    GUID joystickGUID = GUID_NULL;
public:

    enum ButtonType {
        BUTTON_LEFT,
        BUTTON_RIGHT,
    };

    static Input* GetInstance();
    Input() = default;
    Input(Input&) = delete;

    HRESULT Initialize(Window& window);
    /// @brief キーを押した状態 
    static bool IsPushKey(const uint8_t& key);
    /// @briefキーを押した瞬間
    static bool IsTriggerKey(const uint8_t& key);
    /// @briefキーを離した状態
    static bool IsReleaseStateKey(const uint8_t& key);
    /// @briefキーを離した瞬間
    static bool IsReleaseKey(const uint8_t& key);

    /// @brief キーの情報を取得する
    void Update();

    static bool IsPressMouse(uint32_t index);

    static bool IsJoyStickPressButton(uint32_t index);
    static bool IsJoyStickTrigger(uint32_t index);

    static bool GetJoyStickPos(float* x, float* y, ButtonType buttonType);
    static bool GetJoyStickDPadButton(float* x, float* y);

    DIJOYSTATE& GetJoyState() { return joyState_; };

    static Vector2& GetMousePos();
    static float GetMouseWheel();

    static void EyeOperation(Camera& camera);

    static Vector2& GetOffset() { return offset_; }
    static Vector2& GetCurrentPos() { return currentPos_; }
    static Vector3& GetPos() { return pos_; }
    ShericalCoordinate& GetSc() { return shericalCoordinate_; }
private:
    static Input* instance_;
    Window* window_ = nullptr;

    IDirectInputDevice8* keyboard_ = nullptr;
    //全キー入力状態を取得する
    static BYTE key_[256];
    static BYTE preKey_[256];
    //マウス
    IDirectInputDevice8* mouse_ = nullptr;
    static DIMOUSESTATE mouseState_;
    DIMOUSESTATE mouseState_bak_ = {};	// マウス情報(変化検知用)
    static float mouseWheelVol_;
    static bool isDragging_;

    static Vector2 offset_;
    static Vector2 currentPos_;
    static Vector3 pos_;
    static ShericalCoordinate shericalCoordinate_;

    //ゲームパッド
    IDirectInputDevice8* gamePad_ = nullptr;
    static DIJOYSTATE joyState_;
    static float deadZone_;
    static BYTE preJoyButtons_[32];

private:
    static bool NormalizeButtonCount(float* x, float* y, LONG& buttonLX, LONG& buttonLY);
    Input& operator=(Input&) = delete;
    ~Input();
};
