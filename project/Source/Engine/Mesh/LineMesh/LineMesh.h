#pragma once
#include"MeshCommon.h"

class LineMesh : public Primitive
{
public:
   void Create(std::unique_ptr<MeshData> meshData);
   void SetVertex(const Vector3& start, const Vector3& end);
private:

   std::unique_ptr<MeshData> meshData_;
};

