#pragma once

#include<string>//ログの文字列を出力するため
//ファイルの書いたり読んだりするライブラリ　音声の読み込みにも使用する
#include <fstream>

//void Log(const std::string& message);

class LogFile {
private:
    static std::ofstream stream_;
public:
    static void Create();
    static void Log(const std::string& message);
};
