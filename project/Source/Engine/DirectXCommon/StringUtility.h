#pragma once

#include<string>//ログの文字列を出力するため


namespace StringUtility
{
    //string->wstringに変換する関数
    std::wstring ConvertString(const std::string& str);
    //wstring->stringに変換する関数
    std::string ConvertString(const std::wstring& str);


};

