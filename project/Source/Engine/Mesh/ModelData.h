#pragma once
#include<vector>
#include"VertexData.h"
#include"MaterialData.h"

struct ModelData {
    std::vector<VertexData> vertices;
    MaterialData material;
    std::string filename;
};

///// @brief ModelDataの読み込み関数/// @param directoryPath ディレクトリパス/// @param filename ファイル名/// @return　モデルデータ 
//ModelData LoadObjeFile(const std::string& directoryPath, const std::string& filename);