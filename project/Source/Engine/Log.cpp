#include"Log.h"
#include<Windows.h>
#include <filesystem>
#include <chrono>


//ログを出力する関数
void Log(const std::string& message) {
    OutputDebugStringA(message.c_str());
}

void Log(std::ostream& os, const std::string& message) {
    os << message << std::endl;
    OutputDebugStringA(message.c_str());
}

//string->wstringに変換する関数
std::wstring ConvertString(const std::string& str) {
    if (str.empty()) {
        return std::wstring();
    }

    auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
    if (sizeNeeded == 0) {
        return std::wstring();
    }
    std::wstring result(sizeNeeded, 0);
    MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);
    return result;
}

//wstring->stringに変換する関数
std::string ConvertString(const std::wstring& str) {
    if (str.empty()) {
        return std::string();
    }

    auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);
    if (sizeNeeded == 0) {
        return std::string();
    }
    std::string result(sizeNeeded, 0);
    WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded, NULL, NULL);
    return result;
}


std::ofstream LogFile::CreateLogFile() {

    // ログのディレクトリを用意
    std::filesystem::create_directory("logs");
    //現在時刻を取得（UTC時刻）
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    //ログファイルの名前にコンマ何秒はいらないので、削って秒にする
    std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>
        nowSeconds = std::chrono::time_point_cast<std::chrono::seconds>(now);
    //日本時刻(PCの設定時間)に変換
    std::chrono::zoned_time localTime{ std::chrono::current_zone(),nowSeconds };
    //formatを使ってファイル名を決定
    std::string dateString = std::format("{:%Y%m%d_%H%M%S}", localTime);
    //時刻を使ってファイル名を出力
    std::string logFilePath = std::string("logs/") + dateString + "log";
    //ファイルを作って書き込み準備
    std::ofstream logStream(logFilePath);

    return logStream;

}