#include"DrawGrid.h"
#include<numbers>
#include"MakeMatrix.h"
#include"Texture.h"
#include"AABB.h"

std::array<std::unique_ptr<Primitive>, 102> DrawGrid::line_;
std::array <std::unique_ptr<Primitive>, 2>  DrawGrid::cube_;

std::array< std::unique_ptr<Object3d>, 102>  DrawGrid::lineTransforms_;

void DrawGrid::Finalize()
{
    for (auto& l : line_) {
        l.reset();
    }

    for (auto& c : cube_) {
        c.reset();
    }

    for (auto& t : lineTransforms_) {
        t.reset();
    }
}

void DrawGrid::Create()
{
    for (int i = 0; i < line_.size(); ++i) {
        line_[i] = std::make_unique<Primitive>();  
    }

    for (int i = 0; i < 51; ++i) {
        line_[i]->Create(PrimitiveGenerator::CreateLine(Vector3(-25.0f, 0.0f, static_cast<float>(i - 25)), Vector3(25.0f, 0.0f, static_cast<float>(i - 25))));
        line_[i+51]->Create(PrimitiveGenerator::CreateLine(Vector3(static_cast<float>(i - 25), 0.0f, -25.0f), Vector3(static_cast<float>(i - 25), 0.0f, 25.0f)));
    }

    for (int i = 0; i < cube_.size(); ++i) {
        cube_[i] = std::make_unique <Primitive>();
    }

    AABB aabb0 = { { -2.0f / 128.0f,-2.0f / 128.0f,-25.0f }, { 2.0f / 128.0f,2.0f / 128.0f,25.0f } };
    AABB aabb1 = { { -25.0f,-2.0f / 128.0f,-2.0f / 128.0f }, { 25.0f,2.0f / 128.0f,2.0f / 128.0f } };

    cube_[0]->Create(PrimitiveGenerator::CreateCube(aabb0));
    cube_[1]->Create(PrimitiveGenerator::CreateCube(aabb1));

    for (size_t i = 0; i < lineTransforms_.size(); ++i) {
        lineTransforms_[i] = std::make_unique<Object3d>();
        lineTransforms_[i]->Create();
        lineTransforms_[i]->Update();
        lineTransforms_[i]->SetLightMode(kLightModeNone);
        if (i != 25 && i != 76) {
            lineTransforms_[i]->SetMeshAndMaterial(line_[i].get());
        }

        if (i < 50) {
            if ((i) % 10 == 0) {
                lineTransforms_[i]->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
            } else {
                lineTransforms_[i]->SetColor(Vector4(0.5f, 0.5f, 0.5f, 1.0f));
            }

        } else {
            if ((i + 4) % 10 == 0) {
                lineTransforms_[i]->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
            } else {
                lineTransforms_[i]->SetColor(Vector4(0.5f, 0.5f, 0.5f, 1.0f));
            }
        }


    }

    lineTransforms_[27]->SetMeshAndMaterial(cube_[0].get());
    lineTransforms_[27]->SetColor(Vector4(0.0f, 1.0f, 0.0f, 1.0f));
    lineTransforms_[76]->SetMeshAndMaterial(cube_[1].get());
    lineTransforms_[76]->SetColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));
}

void DrawGrid::Draw(Camera& camera) {

    for (int i = 0; i < lineTransforms_.size(); ++i) {
        lineTransforms_[i]->Draw(camera, kBlendModeNone);
    }
}