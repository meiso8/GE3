#pragma once
#define DIRECTINPUT_VERSION 0x0800//DirectXバージョン指定
//　DIRECTINPUT_VERSION　dinput.hのインクルードより上に書くこと。
#include <dinput.h>
#include<cstdint>
#include"Vector2.h"
#include"Window.h"
#include"SphericalCoordinate.h"
#include"Window.h"

#define FPS 60.0f

class Camera;

class Input {
public:
    bool foundJoystick_ = false;
    GUID joystickGUID = GUID_NULL;
public:

    enum ButtonType {
        BUTTON_LEFT,
        BUTTON_RIGHT,
    };

    static Input* GetInstance();
    Input() = default;
    ~Input();

    HRESULT Initialize(Window& window/*, int& fps*/);
    /// @brief キーを押した状態 
    bool IsPushKey(const uint8_t& key);
    /// @briefキーを押した瞬間
    bool IsTriggerKey(const uint8_t& key);
    /// @briefキーを離した状態
    bool IsReleaseStateKey(const uint8_t& key);
    /// @briefキーを離した瞬間
    bool IsReleaseKey(const uint8_t& key);

    /// @brief キーの情報を取得する
    void Update();

    bool IsPressMouse(uint32_t index);

    bool IsJoyStickPressButton(uint32_t index);
    bool IsJoyStickTrigger(uint32_t index);
  
    bool GetJoyStickPos(float* x, float* y, ButtonType buttonType);
    bool GetJoyStickDPadButton(float* x, float* y);

    DIJOYSTATE& GetJoyState() { return joyState_; };

    Vector2& GetMousePos();
    float GetMouseWheel();

    void EyeOperation(Camera& camera);

    Vector2& GetOffset() { return offset_; }
    Vector2& GetCurrentPos() { return currentPos_; }
    Vector3& GetPos() { return pos_; }
    ShericalCoordinate& GetSc() { return shericalCoordinate_; }
private:
    static Input* instance_;
    Window* window_ = nullptr;

    IDirectInputDevice8* keyboard_ = nullptr;
    //全キー入力状態を取得する
    BYTE key_[256] = {};
    BYTE preKey_[256] = {};
    //マウス
    IDirectInputDevice8* mouse_ = nullptr;
    DIMOUSESTATE mouseState_ = {};
    DIMOUSESTATE mouseState_bak_ = {};	// マウス情報(変化検知用)
    float mouseWheelVol_ = 0;
    bool isDragging_ = false;


    Vector2 offset_ = { 0.0f,0.0f };
    Vector2 currentPos_ = { 0.0f };
    Vector3 pos_ = { 0.0f };
    ShericalCoordinate shericalCoordinate_ = { 0.0f,0.0f,0.0f };

    //ゲームパッド
    IDirectInputDevice8* gamePad_ = nullptr;
    DIJOYSTATE joyState_{};
    float deadZone_ = 0.1f;
    BYTE preJoyButtons_[32];

private:
    bool NormalizeButtonCount(float* x, float* y, LONG& buttonLX, LONG& buttonLY);

};
