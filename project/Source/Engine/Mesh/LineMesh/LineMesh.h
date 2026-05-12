#pragma once
#include"MeshCommon.h"

class LineMesh : public Primitive
{
public:
   void CreateLineMesh(std::unique_ptr<MeshData> meshData, const TextureFactory::Handle& textureHandle = TextureFactory::WHITE_1X1);
   void SetVertex(const Vector3& start, const Vector3& end);
private:

   std::unique_ptr<MeshData> meshData_;
};

