#pragma once
#include"Primitive.h"
#include<unordered_map>
#include<string>

class PrimitiveFactory
{
public:
    static Primitive* GetPrimitive(const Primitive::TopologyType& topologyType);
    static Primitive* GetPrimitiveForName(const std::string name);
    static MeshData GetMeshData(const Primitive::TopologyType& topologyType);
    void CreateAllPrimitive();
private:
   static std::unordered_map<std::string, Primitive>primitives_;
};

