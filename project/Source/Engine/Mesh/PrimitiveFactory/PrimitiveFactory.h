#pragma once
#include"Primitive.h"
#include<unordered_map>
#include<string>

class PrimitiveFactory
{
public:
    static Primitive* GetPrimitive(const Primitive::MeshType& topologyType);
    static Primitive* GetPrimitiveForName(const std::string name);
    static MeshData GetMeshData(const Primitive::MeshType& topologyType);
    void CreateAllPrimitive();
    static std::unordered_map<std::string, Primitive>GetPrimitives() { return primitives_; };
    void Finalize();
private:
    void CreateAndSetMeshType(const Primitive::MeshType& topologyType);
    static std::unordered_map<std::string, Primitive>primitives_;
    static std::unordered_map<Primitive::MeshType, std::string>meshType_;
};

