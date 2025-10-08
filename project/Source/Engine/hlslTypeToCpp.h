#pragma once

#include"Vector4.h"
#include"Vector3.h"
#include"Vector2.h"

#ifdef __cplusplus
using float32_t4 = Vector4;
using float32_t3 = Vector3;
using float32_t2 = Vector2;
#endif

//ShaderReflectionで解決する方法もある
//Cbufferの変数名が取れる
//レジスタ番号が取れる
//変数名もとれる
//Cbufferしか取れないのでStructredBufferなどは取れない
//付帯情報がない

///schema型定義　yml(ヤムル) json
//-Material
// -bias
//    -float
//    -min : 0
//    -max : 1
// -color
//   -color4
//  -id
//    -float4

//上記の型定義からファイルを作成する Convertor
//MaterialHllsl.h
//MaterialCpp.h　->jsonを作成してから　ObjectReflection.json
//MaterialEdit.h

//DataDriven データを変えるだけでexe等　変わる