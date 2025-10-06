#include"Log.h"
#include<Windows.h>
#include <filesystem>
#include <chrono>

std::ofstream LogFile::stream_;

//void Log(const std::string& message) {
//    OutputDebugStringA(message.c_str());
//}

void LogFile::Create()
{
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

    stream_.open(logFilePath);
}

//ログを出力する関数
void LogFile::Log(const std::string& message) {
    //OSにログを出力する
    stream_ << message << std::endl;
    OutputDebugStringA(message.c_str());
}
