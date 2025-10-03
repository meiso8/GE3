#pragma once  
#include <Windows.h>  
#include<cstdint>//int32_tを使うため
#include<string>

class Window
{
public:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
public:
    void Create(const std::wstring& title, const int32_t& clientWidth, const int32_t& clientHeight);
    HINSTANCE GetHInstance()const { return wc_.hInstance; }
    HWND GetHwnd() const{ return hwnd_; }
    int32_t GetClientWidth() const {
        return clientWidth_;
    }
    int32_t GetClientHeight() const {
        return clientHeight_;
    }
    
    //メッセージを受け取る処理
    bool ProcessMassage();

    void Finalize();
private:
    /// @brief ウィンドウクラス  
    WNDCLASS wc_{};
    /// @brief ウィンドウハンドル  
    HWND hwnd_ = nullptr;
    //クライアント領域のサイズ  
   int32_t clientWidth_{};
    int32_t clientHeight_{};
};
