#include"DrawGrid.h"
#include<numbers>
#include"MakeMatrix.h"
#include"Texture.h"
#include"AABB.h"

 std::array <std::unique_ptr<Primitive>, 2> DrawGrid::cube_;
std::array< std::unique_ptr<Object3d>, 2> DrawGrid::cubes_;
std::array< std::unique_ptr<LineObject3d>, 102>DrawGrid::lineTransforms2_;

void DrawGrid::Finalize()
{
    for (auto& c : cubes_) {
        c.reset();
    }

    for (auto& c : cube_) {
        c.reset();
    }

    for (auto& t : lineTransforms2_) {
        t.reset();
    }
}

void DrawGrid::Create()
{

    for (auto& line : lineTransforms2_) {
        line = std::make_unique<LineObject3d>();
    }

    for (int i = 0; i < 51; ++i) {
        lineTransforms2_[i]->Create(Vector3(-25.0f, 0.0f, static_cast<float>(i - 25)), Vector3(25.0f, 0.0f, static_cast<float>(i - 25)));
        lineTransforms2_[i+51]->Create(Vector3(static_cast<float>(i - 25), 0.0f, -25.0f), Vector3(static_cast<float>(i - 25), 0.0f, 25.0f));
    }

    for (int i = 0; i < cube_.size(); ++i) {
        cube_[i] = std::make_unique <Primitive>();
    }

    AABB aabb0 = { { -2.0f / 128.0f,-2.0f / 128.0f,-25.0f }, { 2.0f / 128.0f,2.0f / 128.0f,25.0f } };
    AABB aabb1 = { { -25.0f,-2.0f / 128.0f,-2.0f / 128.0f }, { 25.0f,2.0f / 128.0f,2.0f / 128.0f } };

    cube_[0]->Create(PrimitiveGenerator::CreateCube(aabb0));
    cube_[1]->Create(PrimitiveGenerator::CreateCube(aabb1));

    for (auto& cubeTransform : cubes_) {
        cubeTransform = std::make_unique<Object3d>();
        cubeTransform->Create();
        cubeTransform->SetLightMode(kLightModeNone);
    }

    for (size_t i = 0; i < lineTransforms2_.size(); ++i) {

        lineTransforms2_[i]->Update();

        if (i < 50) {
            if ((i) % 10 == 0) {
                lineTransforms2_[i]->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
            } else {
                lineTransforms2_[i]->SetColor(Vector4(0.5f, 0.5f, 0.5f, 1.0f));
            }

        } else {
            if ((i + 4) % 10 == 0) {
                lineTransforms2_[i]->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
            } else {
                lineTransforms2_[i]->SetColor(Vector4(0.5f, 0.5f, 0.5f, 1.0f));
            }
        }
    }

    cubes_[0]->SetMeshAndMaterial(cube_[0].get());
    cubes_[0]->SetColor(Vector4(0.0f, 1.0f, 0.0f, 1.0f));
    cubes_[1]->SetMeshAndMaterial(cube_[1].get());
    cubes_[1]->SetColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));
}

void DrawGrid::Draw(Camera& camera) {

    for (auto& line : lineTransforms2_) {
        line->Draw(camera,false);
    }

    for (auto& cubeTransform : cubes_) {
        cubeTransform->Draw(camera, kBlendModeNone);
    }
}