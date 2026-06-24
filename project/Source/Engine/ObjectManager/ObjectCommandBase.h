#pragma once
class ObjectCommandBase
{
public:
    //実行処理の純粋仮想関数
    virtual void Do() = 0;
    //巻き戻し処理の純粋仮想関数
    virtual void UnDo() = 0;
private:
};


