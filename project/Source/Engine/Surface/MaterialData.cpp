#include"MaterialData.h"
#include <filesystem>
#include <fstream>//ファイルの書いたり読んだりするライブラリ
#include<cassert> //assertも利用するため

MaterialData LoadMaterialTemplateFile(const std::string& dirextoryPath, const std::string& filename) {
   
    //1.中で必要となる変数の宣言
    MaterialData materialData;//構築するMaterialData
    std::string line;//ファイルから読んだ1行を格納するもの
    std::ifstream file(dirextoryPath + "/" + filename);//ファイルを開く
    assert(file.is_open());   //2.ファイルを開く
 
    
    //3.実際にファイルを読み、MaterialDatを構築していく
    while (std::getline(file, line)) {
        std::string identifier;
        std::istringstream s(line);
        s >> identifier;

        //identifierに応じた処理
        if(identifier == "map_Kd"){
            std::string textureFilename;
            s >> textureFilename;
            //連結してファイルパスにする
            materialData.textureFilePath = dirextoryPath + "/" + textureFilename;
        }

    }

    //4.MaterialDataを返す

    return materialData;
}