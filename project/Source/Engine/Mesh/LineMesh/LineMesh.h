#pragma once
#include"Primitive.h"

class LineMesh : public Primitive
{
public:
    LineMesh();
    ~LineMesh();

    void Create(const Vector3& startPos, const Vector3& endPos);
    void SetVertex(const Vector3& start, const Vector3& end);

private:

    std::unique_ptr<MeshData> meshData_;
};

