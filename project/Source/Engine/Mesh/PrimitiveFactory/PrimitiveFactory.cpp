#include "PrimitiveFactory.h"
 
std::unordered_map<std::string, Primitive>PrimitiveFactory::primitives_;
std::unordered_map<Primitive::MeshType, std::string>PrimitiveFactory::meshType_;

Primitive* PrimitiveFactory::GetPrimitiveForName(const std::string name)
{
    if (primitives_.contains(name)) {
        return &primitives_.at(name);
    };

    //名前がなかった。
    return nullptr;
}

Primitive* PrimitiveFactory::GetPrimitive(const Primitive::MeshType& topologyType)
{ 
   return GetPrimitiveForName(meshType_[topologyType]);
}

void PrimitiveFactory::CreateAllPrimitive()
{
    CreateAndSetMeshType(Primitive::kPlane);
    CreateAndSetMeshType(Primitive::kCube);
    CreateAndSetMeshType(Primitive::kSphere);
    CreateAndSetMeshType(Primitive::kRing);
    CreateAndSetMeshType(Primitive::kCylinder);
}

PrimitiveFactory::~PrimitiveFactory()
{
    for (auto& [name, primitive] : primitives_) {
        primitive.~Primitive();
    }

    primitives_.clear();
}

void PrimitiveFactory::CreateAndSetMeshType(const Primitive::MeshType& topologyType)
{
    MeshData mesh = GetMeshData(topologyType);
    primitives_[mesh.meshName].Create(mesh);
    meshType_[topologyType] = mesh.meshName;
}

MeshData PrimitiveFactory::GetMeshData(const Primitive::MeshType& topologyType)
{
    switch (topologyType)
    {
    case Primitive::kPlane:
        return PrimitiveGenerator::CreatePlane({ 2.0f,2.0f });
        break;
    case Primitive::kCube:
        return  PrimitiveGenerator::CreateCube();
        break;
    case Primitive::kSphere:
        return PrimitiveGenerator::CreateSphere();
        break;
    case Primitive::kRing:
        return  PrimitiveGenerator::CreateRing();
        break;
    case Primitive::kCylinder:
        return PrimitiveGenerator::CreateCylinder();
        break;
    default:
        return PrimitiveGenerator::CreatePlane({ 1.0f,1.0f });
        break;
    }
}
