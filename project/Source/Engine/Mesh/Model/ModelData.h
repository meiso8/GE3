#pragma once

#include"VertexData.h"
#include"MaterialData.h"
#include"Node.h"
#include<cstdint>
#include<map>
#include<d3d12.h>
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
};

struct ModelData :public MeshData {

    std::map<std::string, JointWeightData> skinClusterData;
    MaterialData material;
    std::string filePath;
    Node rootNode;
};

