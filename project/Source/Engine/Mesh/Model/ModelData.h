#pragma once

#include"VertexData.h"
#include"MaterialData.h"
#include"Node.h"
#include<cstdint>
#include<map>
#include<d3d12.h>
#include<string>
#include<Animation.h>

struct VertexWeightData {
    float weight;
    uint32_t vertexIndex;
};

struct JointWeightData {
    Matrix4x4 inverseBindPoseMatrix;
    std::vector<VertexWeightData> vertexWeights;
};

struct MeshData {
    std::vector<VertexData> vertices;
    std::vector<uint32_t> indices;
    // デフォルトは三角ポリゴン
    D3D_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    std::string meshName = "unKnown";
};

struct MeshSection {
    uint32_t indexStart;    // このマテリアルの描画を開始するインデックス位置
    uint32_t indexCount;    // 描画するインデックス数
    std::string materialName; // ★追加：このセクションが使用するマテリアル名
};

struct ModelData :public MeshData {

    std::map<std::string, JointWeightData> skinClusterData;
    std::vector<MeshSection> sections; // ★追加：マテリアルごとの範囲情報
    std::map<std::string, MaterialData>materials;
    Node rootNode;
    std::map <std::string, Animation> animations_;
    std::string directoryPath_;
};

