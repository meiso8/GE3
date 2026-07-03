#include "Model.h"

Model::~Model()
{
    Primitive::~Primitive();
}

void Model::CreateModel()
{
    Create(*modelData_);
}
