#include "PrimitiveFactory.h"
 
std::unordered_map<std::string, Primitive>PrimitiveFactory::primitives_;

Primitive* PrimitiveFactory::GetPrimitive(const Primitive::TopologyType& topologyType)
{
    
    switch (topologyType)
    {
    case Primitive::kPlane:
       return GetPrimitiveForName("Plane");
        break;
    case Primitive::kCube:
        return  GetPrimitiveForName("Cube");
        break;
    case Primitive::kSphere:
        return  GetPrimitiveForName("Sphere");
        break;
    case Primitive::kRing:
        return  GetPrimitiveForName("Ring");
        break;
    case Primitive::kCylinder:
        return  GetPrimitiveForName("Cylinder");
        break;
    default:
        return  GetPrimitiveForName("Plane");
        break;
    }

}

void PrimitiveFactory::CreateAllPrimitive()
{
    primitives_["Plane"].Create(PrimitiveGenerator::CreatePlane({ 1.0f,1.0f }));
    primitives_["Cube"].Create(PrimitiveGenerator::CreateCube());
    primitives_["Sphere"].Create(PrimitiveGenerator::CreateSphere());
    primitives_["Ring"].Create(PrimitiveGenerator::CreateRing());
    primitives_["Cylinder"].Create(PrimitiveGenerator::CreateCylinder());
}

Primitive* PrimitiveFactory::GetPrimitiveForName(const std::string name)
{
    if (primitives_.contains(name)) { 
        return &primitives_.at(name);
    };
    
    //名前がなかった。
    return nullptr;
}

MeshData PrimitiveFactory::GetMeshData(const Primitive::TopologyType& topologyType)
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
