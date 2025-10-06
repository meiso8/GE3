#include "DXGIFactory.h"
#include<cassert> //assertも利用するため

void DXGIFactory::Create() {

    //HRESULTはWindow系のエラーコードであり、
    //関数が成功したかどうかをSUCCEEDEDマクロで判定できる
    HRESULT result= CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
    //初期化の根本的な部分でエラーが出た場合はプログラムが間違っているか、どうにもできない場合が多いのでassertにしておく
    assert(SUCCEEDED(result));

}


