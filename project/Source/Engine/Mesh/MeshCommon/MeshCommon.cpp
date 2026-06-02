#include "MeshCommon.h"
#include"DirectXCommon.h"
#include"Texture.h"
#include"PSO.h"
#include"SRVmanager/SrvManager.h"
#include<numbers>


MeshData PrimitiveGenerator::CreateCube(const AABB& aabb)
{

    MeshData data;

    //　================頂点データの作成=============================

    data.vertices.resize(20);

    Vector3 vertexData[8] = {};

    vertexData[0] = { aabb.max.x,aabb.max.y,aabb.max.z };  //上面奥右
    vertexData[1] = { aabb.min.x,aabb.max.y,aabb.max.z };    //上面奥左   
    vertexData[2] = { aabb.max.x,aabb.min.y,aabb.max.z };    //底面奥右     
    vertexData[3] = { aabb.min.x,aabb.min.y,aabb.max.z };  //底面奥左
    vertexData[4] = { aabb.max.x,aabb.max.y,aabb.min.z };  //上面手前右
    vertexData[5] = { aabb.min.x,aabb.max.y,aabb.min.z };     //上面手前左
    vertexData[6] = { aabb.max.x,aabb.min.y,aabb.min.z };    //底面手前右
    vertexData[7] = { aabb.min.x,aabb.min.y,aabb.min.z };        //底面手前左


    {
        //上面手前右
        data.vertices[0].position = {
            vertexData[4].x,
            vertexData[4].y,
            vertexData[4].z,1.0f };
        data.vertices[0].texcoord = { 1.0f,1.0f };
        data.vertices[0].normal = vertexData[4];//法線
        //上面手前左
        data.vertices[1].position = {
            vertexData[5].x,
            vertexData[5].y,
            vertexData[5].z,1.0f };
        data.vertices[1].texcoord = { 0.0f,1.0f };
        data.vertices[1].normal = vertexData[5];//法線

        //上面奥右
        data.vertices[2].position = {
           vertexData[0].x,
           vertexData[0].y,
           vertexData[0].z,1.0f };
        data.vertices[2].texcoord = { 1.0f,0.0f };
        data.vertices[2].normal = vertexData[0];//法線

        //上面奥左
        data.vertices[3].position = {
          vertexData[1].x,
          vertexData[1].y,
          vertexData[1].z,1.0f };
        data.vertices[3].texcoord = { 0.0f,0.0f };
        data.vertices[3].normal = vertexData[1];//法線

    }

    {
        //底面手前右
        data.vertices[4].position = {
           vertexData[6].x,
           vertexData[6].y,
           vertexData[6].z,1.0f };
        data.vertices[4].texcoord = { 1.0f,1.0f };
        data.vertices[4].normal = vertexData[6];//法線
        //底面手前左
        data.vertices[5].position = {
            vertexData[7].x,
            vertexData[7].y,
            vertexData[7].z,1.0f };
        data.vertices[5].texcoord = { 0.0f,1.0f };
        data.vertices[5].normal = vertexData[7];//法線

        //底面奥右
        data.vertices[6].position = {
             vertexData[2].x,
             vertexData[2].y,
             vertexData[2].z,1.0f };
        data.vertices[6].texcoord = { 1.0f,0.0f };
        data.vertices[6].normal = vertexData[2];//法線

        //底面奥左
        data.vertices[7].position = {
           vertexData[3].x,
           vertexData[3].y,
           vertexData[3].z,
           1.0f };
        data.vertices[7].texcoord = { 0.0f,0.0f };
        data.vertices[7].normal = vertexData[3];//法線

    }


    {

        //上面手前左
        data.vertices[8].position = data.vertices[1].position;
        data.vertices[8].texcoord = { 0.0f,0.0f };
        data.vertices[8].normal = data.vertices[1].normal;//法線

        //上面手前右
        data.vertices[9].position = data.vertices[0].position;
        data.vertices[9].texcoord = { 1.0f,0.0f };
        data.vertices[9].normal = data.vertices[0].normal;//法線

    }

    {
        //上面手前右
        data.vertices[10].position = data.vertices[0].position;
        data.vertices[10].texcoord = { 0.0f,0.0f };
        data.vertices[10].normal = data.vertices[0].normal;//法線

        //底面手前右
        data.vertices[11].position = data.vertices[4].position;
        data.vertices[11].texcoord = { 0.0f,1.0f };
        data.vertices[11].normal = data.vertices[4].normal;//法線

        //底面奥右
        data.vertices[12].position = data.vertices[6].position;
        data.vertices[12].texcoord = { 1.0f,1.0f };
        data.vertices[12].normal = data.vertices[6].normal;//法線

    }

    {
        //上面奥右
        data.vertices[13].position = data.vertices[2].position;
        data.vertices[13].texcoord = { 0.0f,0.0f };
        data.vertices[13].normal = data.vertices[2].normal;//法線

        //上面奥左 かぶり
        data.vertices[14].position = data.vertices[3].position;
        data.vertices[14].texcoord = { 1.0f,0.0f };
        data.vertices[14].normal = data.vertices[3].normal;//法線

        //底面奥右
        data.vertices[15].position = data.vertices[6].position;
        data.vertices[15].texcoord = { 0.0f,1.0f };
        data.vertices[15].normal = data.vertices[6].normal;//法線

        //底面奥左
        data.vertices[16].position = data.vertices[7].position;
        data.vertices[16].texcoord = { 1.0f,1.0f };
        data.vertices[16].normal = data.vertices[7].normal;//法線
    }

    {

        //上面手前左
        data.vertices[17].position = data.vertices[1].position;
        data.vertices[17].texcoord = { 1.0f,0.0f };
        data.vertices[17].normal = data.vertices[1].normal;//法線

        //底面奥左
        data.vertices[18].position = data.vertices[7].position;
        data.vertices[18].texcoord = { 0.0f,1.0f };
        data.vertices[18].normal = data.vertices[7].normal;//法線

        //底面手前左
        data.vertices[19].position = data.vertices[5].position;
        data.vertices[19].texcoord = { 1.0f,1.0f };
        data.vertices[19].normal = data.vertices[5].normal;//法線

    }

    //======================= インデックスデータも同様に詰める ================================



    data.indices.resize(36);

    //頂点数を削減
     //上面
    data.indices[0] = 1;   data.indices[1] = 3;   data.indices[2] = 0;
    data.indices[3] = 0;   data.indices[4] = 3;   data.indices[5] = 2;

    // 底
    data.indices[6] = 7;   data.indices[7] = 5;   data.indices[8] = 6;
    data.indices[9] = 6;   data.indices[10] = 5;   data.indices[11] = 4;

    // front face
    data.indices[12] = 5;   data.indices[13] = 8;   data.indices[14] = 4;
    data.indices[15] = 4;   data.indices[16] = 8;   data.indices[17] = 9;

    // Right face
    data.indices[18] = 11;   data.indices[19] = 10;   data.indices[20] = 12;
    data.indices[21] = 12;   data.indices[22] = 10;   data.indices[23] = 2;

    // back face
    data.indices[24] = 15;   data.indices[25] = 13;   data.indices[26] = 16;
    data.indices[27] = 16;   data.indices[28] = 13;   data.indices[29] = 14;

    // left face
    data.indices[30] = 18;   data.indices[31] = 3;   data.indices[32] = 19;
    data.indices[33] = 19;   data.indices[34] = 3;   data.indices[35] = 17;

    //トポロジーの設定
    data.topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    return data;
}

MeshData PrimitiveGenerator::CreateSkyBox(const AABB& aabb)
{

    MeshData data;
    data.vertices.resize(20);

    Vector3 vertexData[8] = {};

    vertexData[0] = { aabb.max.x,aabb.max.y,aabb.max.z };  //上面奥右
    vertexData[1] = { aabb.min.x,aabb.max.y,aabb.max.z };    //上面奥左   
    vertexData[2] = { aabb.max.x,aabb.min.y,aabb.max.z };    //底面奥右     
    vertexData[3] = { aabb.min.x,aabb.min.y,aabb.max.z };  //底面奥左
    vertexData[4] = { aabb.max.x,aabb.max.y,aabb.min.z };  //上面手前右
    vertexData[5] = { aabb.min.x,aabb.max.y,aabb.min.z };     //上面手前左
    vertexData[6] = { aabb.max.x,aabb.min.y,aabb.min.z };    //底面手前右
    vertexData[7] = { aabb.min.x,aabb.min.y,aabb.min.z };        //底面手前左


    {
        //上面手前右
        data.vertices[0].position = {
          vertexData[4].x,
          vertexData[4].y,
          vertexData[4].z,1.0f };
        data.vertices[0].normal = vertexData[4];//法線
        //上面手前左
        data.vertices[1].position = {
          vertexData[5].x,
          vertexData[5].y,
          vertexData[5].z,1.0f };
        data.vertices[1].normal = vertexData[5];//法線

        //上面奥右
        data.vertices[2].position = {
         vertexData[0].x,
         vertexData[0].y,
         vertexData[0].z,1.0f };

        data.vertices[2].normal = vertexData[0];//法線

        //上面奥左
        data.vertices[3].position = {
         vertexData[1].x,
         vertexData[1].y,
         vertexData[1].z,1.0f };
        data.vertices[3].normal = vertexData[1];//法線

    }

    {
        //底面手前右
        data.vertices[4].position = {
          vertexData[6].x,
          vertexData[6].y,
          vertexData[6].z,1.0f };
        data.vertices[4].normal = vertexData[6];//法線
        //底面手前左
        data.vertices[5].position = {
          vertexData[7].x,
          vertexData[7].y,
          vertexData[7].z,1.0f };
        data.vertices[5].normal = vertexData[7];//法線

        //底面奥右
        data.vertices[6].position = {
          vertexData[2].x,
          vertexData[2].y,
          vertexData[2].z,1.0f };
        data.vertices[6].normal = vertexData[2];//法線

        //底面奥左
        data.vertices[7].position = {
         vertexData[3].x,
         vertexData[3].y,
         vertexData[3].z,
          1.0f };
        data.vertices[7].normal = vertexData[3];//法線
    }


    {
        //上面手前左
        data.vertices[8].position = data.vertices[1].position;
        data.vertices[8].normal = data.vertices[1].normal;//法線
        //上面手前右
        data.vertices[9].position = data.vertices[0].position;
        data.vertices[9].normal = data.vertices[0].normal;//法線

    }

    {
        //上面手前右
        data.vertices[10].position = data.vertices[0].position;
        data.vertices[10].normal = data.vertices[0].normal;//法線

        //底面手前右
        data.vertices[11].position = data.vertices[4].position;
        data.vertices[11].normal = data.vertices[4].normal;//法線

        //底面奥右
        data.vertices[12].position = data.vertices[6].position;
        data.vertices[12].normal = data.vertices[6].normal;//法線

    }

    {
        //上面奥右
        data.vertices[13].position = data.vertices[2].position;
        data.vertices[13].normal = data.vertices[2].normal;//法線

        //上面奥左 かぶり
        data.vertices[14].position = data.vertices[3].position;
        data.vertices[14].normal = data.vertices[3].normal;//法線

        //底面奥右
        data.vertices[15].position = data.vertices[6].position;
        data.vertices[15].normal = data.vertices[6].normal;//法線

        //底面奥左
        data.vertices[16].position = data.vertices[7].position;
        data.vertices[16].normal = data.vertices[7].normal;//法線
    }

    {
        //上面手前左
        data.vertices[17].position = data.vertices[1].position;
        data.vertices[17].normal = data.vertices[1].normal;//法線

        //底面奥左
        data.vertices[18].position = data.vertices[7].position;
        data.vertices[18].normal = data.vertices[7].normal;//法線

        //底面手前左
        data.vertices[19].position = data.vertices[5].position;
        data.vertices[19].normal = data.vertices[5].normal;//法線

    }

    for (int i = 0; i < 20; i++) {
        //適当に埋めとく
        data.vertices[i].texcoord = { 0.0f,0.0f };
    }
    data.indices.resize(36);
    //頂点数を削減
    // 上面
    data.indices[0] = 1;   data.indices[1] = 0;   data.indices[2] = 3;
    data.indices[3] = 0;   data.indices[4] = 2;   data.indices[5] = 3;

    // tei
    data.indices[6] = 7;   data.indices[7] = 6;   data.indices[8] = 5;
    data.indices[9] = 6;   data.indices[10] = 4;   data.indices[11] = 5;

    // front face
    data.indices[12] = 5;   data.indices[13] = 4;   data.indices[14] = 8;
    data.indices[15] = 4;   data.indices[16] = 9;   data.indices[17] = 8;

    // Right face
    data.indices[18] = 11;   data.indices[19] = 12;   data.indices[20] = 10;
    data.indices[21] = 12;   data.indices[22] = 2;   data.indices[23] = 10;

    // back face
    data.indices[24] = 15;   data.indices[25] = 16;   data.indices[26] = 13;
    data.indices[27] = 16;   data.indices[28] = 14;   data.indices[29] = 13;

    // left face
    data.indices[30] = 18;   data.indices[31] = 19;   data.indices[32] = 3;
    data.indices[33] = 19;   data.indices[34] = 17;   data.indices[35] = 3;


    return data;
}

MeshData PrimitiveGenerator::CreateLine(const Vector3& start, const Vector3& end)
{

    MeshData data;

    data.vertices.resize(2);

    data.vertices[0].position = { start.x,start.y,start.z,1.0f };//左下
    data.vertices[0].texcoord = { 0.0f,1.0f };
    data.vertices[0].normal = { data.vertices[0].position.x,  data.vertices[0].position.y,   data.vertices[0].position.z };//法線
    data.vertices[1].position = { end.x,end.y,end.z,1.0f };//右下
    data.vertices[1].texcoord = { 0.0f,0.0f };
    data.vertices[1].normal = { data.vertices[1].position.x,  data.vertices[1].position.y,   data.vertices[1].position.z };

    // ラインの場合はインデックスを使わないか、0,1と設定する

    //トポロジーの設定
    data.topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;

    return data;
}

MeshData PrimitiveGenerator::CreateCircle(const Circle& circle, const uint32_t kSubdivision)
{

    MeshData data;

    //　=========================//頂点の作成//==================================
    data.vertices.resize(kSubdivision + 1);

    //緯度の方向に分割　-pi/2 ~ pi/2
    const float pi = std::numbers::pi_v<float>;
    const float kLonEvery = 2.0f * pi / float(kSubdivision);

    //経度の方向に分割 0 ~ 2*pi
    for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
        float lon = lonIndex * kLonEvery;//現在の経度
        Vector2 uv = { float(lonIndex) / float(kSubdivision),
            1.0f };
        data.vertices[lonIndex].position.x = std::cos(lon) * circle.radius + circle.center.x;
        data.vertices[lonIndex].position.y = circle.center.y;
        data.vertices[lonIndex].position.z = std::sin(lon) * circle.radius + circle.center.z;
        data.vertices[lonIndex].position.w = 1.0f;
        data.vertices[lonIndex].texcoord = uv;
    }

    //ここが中心点
    data.vertices[kSubdivision].position.x = circle.center.x;
    data.vertices[kSubdivision].position.y = circle.center.y;
    data.vertices[kSubdivision].position.z = circle.center.z;
    data.vertices[kSubdivision].position.w = 1.0f;
    data.vertices[kSubdivision].texcoord = { 0.0f,0.0f };

    for (uint32_t index = 0; index < kSubdivision + 1; ++index) {

        data.vertices[index].normal = {
        data.vertices[index].position.x,
        data.vertices[index].position.y,
        data.vertices[index].position.z
        };
    }


    //　=========================//インデックスの作成//==================================

    data.indices.resize(kSubdivision * 3);
    uint32_t index = 0;

    for (uint32_t i = 0; i < data.indices.size(); i += 3) {

        data.indices[i] = index + 1;
        data.indices[i + 1] = index;

        if (index == kSubdivision - 1) {
            data.indices[i] = 0;
            data.indices[i + 1] = index;
        }

        data.indices[i + 2] = kSubdivision;

        index++;
    }

    //　===========================================================

    //三角トポロジ
    data.topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    return data;
}

MeshData PrimitiveGenerator::CreatePlane(const Vector2& size)
{
    MeshData data;

    data.vertices.resize(4);

    Vector2 halfSize = size * 0.5f;
    data.vertices[0].position = { -halfSize.x,-halfSize.y,0.0f,1.0f };//左下
    data.vertices[1].position = { -halfSize.x,halfSize.y,0.0f,1.0f };//左上
    data.vertices[2].position = { halfSize.x,halfSize.y,0.0f,1.0f };//右上
    data.vertices[3].position = { halfSize.x,-halfSize.y,0.0f,1.0f };//右下

    data.vertices[0].texcoord = { 0.0f, 1.0f }; //左下
    data.vertices[1].texcoord = { 0.0f,0.0f };  //左上
    data.vertices[2].texcoord = { 1.0f,0.0f };  //右下
    data.vertices[3].texcoord = { 1.0f,1.0f };  //右上

    data.vertices[0].normal = { 0.0f,0.0f,1.0f };//法線
    data.vertices[1].normal = { 0.0f,0.0f,1.0f };//法線
    data.vertices[2].normal = { 0.0f,0.0f,1.0f };//法線
    data.vertices[3].normal = { 0.0f,0.0f,1.0f };//法線

    //　=========================//インデックスの作成//==================================

    data.indices.resize(6);
    //頂点数を削減
    data.indices[0] = 0;
    data.indices[1] = 1;
    data.indices[2] = 2;

    data.indices[3] = 2;
    data.indices[4] = 3;
    data.indices[5] = 0;

    //三角トポロジ
    data.topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    return data;
}

MeshData PrimitiveGenerator::CreateRing(const float innerRadius, const float outerRadius, const uint32_t kRingDivide)
{

    MeshData data;

    data.vertices.resize(kRingDivide * 4 + 1);

    //緯度の方向に分割　-pi/2 ~ pi/2
    const float pi = std::numbers::pi_v<float>;
    const float radianPreDivide = 2.0f * pi / float(kRingDivide);

    for (int index = 0; index < int(kRingDivide); ++index) {
        float sin = std::sin(index * radianPreDivide);
        float cos = std::cos(index * radianPreDivide);
        float sinNext = std::sin((index + 1) * radianPreDivide);
        float cosNext = std::cos((index + 1) * radianPreDivide);
        float u = float(index) / float(kRingDivide);
        float uNext = float(index + 1) / float(kRingDivide);

        int indexes = 4 * index;

        data.vertices[indexes].position =
        { -sin * outerRadius, cos * outerRadius, 0.0f, 1.0f };
        data.vertices[indexes].texcoord = { u,0.0f };

        indexes++;

        data.vertices[indexes].position =
        { -sinNext * outerRadius, cosNext * outerRadius, 0.0f, 1.0f };
        data.vertices[indexes].texcoord = { uNext,0.0f };

        indexes++;

        data.vertices[indexes].position =
        { -sin * innerRadius, cos * innerRadius, 0.0f, 1.0f };
        data.vertices[indexes].texcoord = { u, 1.0f };

        indexes++;

        data.vertices[indexes].position =
        { -sinNext * innerRadius, cosNext * innerRadius, 0.0f, 1.0f };
        data.vertices[indexes].texcoord = { uNext, 1.0f };
    }

    for (int i = 0; i < int(kRingDivide * 4); ++i) {
        const auto& pos = data.vertices[i].position;
        data.vertices[i].normal = { pos.x,pos.y,pos.z };
    }

    data.indices.resize(kRingDivide * 6);

    uint32_t indexCount = 0;

    for (uint32_t i = 0; i < kRingDivide; ++i) {
        int vertexIndex = i * 4;
        data.indices[indexCount++] = vertexIndex;
        data.indices[indexCount++] = vertexIndex + 1;
        data.indices[indexCount++] = vertexIndex + 2;

        data.indices[indexCount++] = vertexIndex + 2;
        data.indices[indexCount++] = vertexIndex + 1;
        data.indices[indexCount++] = vertexIndex + 3;
    }

    //三角トポロジ
    data.topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    return data;
}

MeshData PrimitiveGenerator::CreateSphere(const Sphere& sphere, const uint32_t kSubdivision)
{
    MeshData data;

    data.vertices.resize(6 * kSubdivision * kSubdivision);

    const float pi = std::numbers::pi_v<float>;
    const float kLonEvery = 2.0f * pi / float(kSubdivision);
    const float kLatEvery = pi / float(kSubdivision);

    for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
        float lat = -pi / 2.0f + kLatEvery * latIndex;//現在の緯度
        //経度の方向に分割 0 ~ 2*pi
        for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
            //書き込む最初の場所
            uint32_t startIndex = (latIndex * kSubdivision + lonIndex) * 6;
            float lon = lonIndex * kLonEvery;//現在の経度

            Vector2 uv = { float(lonIndex) / float(kSubdivision),
                1.0f - float(latIndex) / float(kSubdivision) };

            //a   
            data.vertices[startIndex].position.x = std::cos(lat) * std::cos(lon);
            data.vertices[startIndex].position.y = std::sin(lat);
            data.vertices[startIndex].position.z = std::cos(lat) * std::sin(lon);
            data.vertices[startIndex].position.w = 1.0f;
            data.vertices[startIndex].texcoord = uv;

            //b
            data.vertices[startIndex + 1].position.x = std::cos(lat + kLatEvery) * std::cos(lon);
            data.vertices[startIndex + 1].position.y = std::sin(lat + kLatEvery);
            data.vertices[startIndex + 1].position.z = std::cos(lat + kLatEvery) * std::sin(lon);
            data.vertices[startIndex + 1].position.w = 1.0f;
            data.vertices[startIndex + 1].texcoord = { uv.x,
                uv.y - 1.0f / float(kSubdivision) };

            //c
            data.vertices[startIndex + 2].position.x = std::cos(lat) * std::cos(lon + kLonEvery);
            data.vertices[startIndex + 2].position.y = std::sin(lat);
            data.vertices[startIndex + 2].position.z = std::cos(lat) * std::sin(lon + kLonEvery);
            data.vertices[startIndex + 2].position.w = 1.0f;
            data.vertices[startIndex + 2].texcoord = { uv.x + 1.0f / float(kSubdivision),
                 uv.y };

            //c
            data.vertices[startIndex + 3] = data.vertices[startIndex + 2];

            //b
            data.vertices[startIndex + 4] = data.vertices[startIndex + 1];

            //d
            data.vertices[startIndex + 5].position.x = std::cos(lat + kLatEvery) * std::cos(lon + kLonEvery);
            data.vertices[startIndex + 5].position.y = std::sin(lat + kLatEvery);
            data.vertices[startIndex + 5].position.z = std::cos(lat + kLatEvery) * std::sin(lon + kLonEvery);
            data.vertices[startIndex + 5].position.w = 1.0f;
            data.vertices[startIndex + 5].texcoord = { uv.x + 1.0f / float(kSubdivision),
                 uv.y - 1.0f / float(kSubdivision) };

            for (int i = 0; i < 6; ++i) {
                data.vertices[startIndex + i].position.x = (data.vertices[startIndex + i].position.x + sphere.center.x) * sphere.radius;
                data.vertices[startIndex + i].position.y = (data.vertices[startIndex + i].position.y + sphere.center.y) * sphere.radius;
                data.vertices[startIndex + i].position.z = (data.vertices[startIndex + i].position.z + sphere.center.z) * sphere.radius;
                data.vertices[startIndex + i].normal = { data.vertices[startIndex + i].position.x , data.vertices[startIndex + i].position.y,data.vertices[startIndex + i].position.z };
            }
        }
    }

    //三角トポロジ
    data.topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    return data;

}

MeshData PrimitiveGenerator::CreateCylinder(const bool isFlip, const float topRadius, const float bottomRadius, const float height, const uint32_t cylinderDivide)
{
    MeshData data;

    const float radianPreDivide = 2.0f * std::numbers::pi_v<float> / float(cylinderDivide);

    data.vertices.resize(cylinderDivide * 6);

    for (uint32_t index = 0; index < cylinderDivide; ++index) {
        float sin = std::sin(index * radianPreDivide);
        float cos = std::cos(index * radianPreDivide);
        float sinNext = std::sin((index + 1) * radianPreDivide);
        float cosNext = std::cos((index + 1) * radianPreDivide);
        float u = float(index) / float(cylinderDivide);
        float uNext = float(index + 1) / float(cylinderDivide);

        uint32_t i = index * 6;
        //上辺左
        data.vertices[i].position = { -sin * topRadius,height,cos * topRadius,1.0f };
        data.vertices[i].texcoord = { u,0.0f };
        data.vertices[i].normal = { -sin,0.0f,cos };
        //上辺右
        data.vertices[i + 1].position = { -sinNext * topRadius,height,cosNext * topRadius,1.0f };
        data.vertices[i + 1].texcoord = { uNext,0.0f };
        data.vertices[i + 1].normal = { -sinNext,0.0f,cosNext };
        //底辺左
        data.vertices[i + 2].position = { -sin * bottomRadius,0.0f,cos * bottomRadius,1.0f };
        data.vertices[i + 2].texcoord = { u,1.0f };
        data.vertices[i + 2].normal = { -sin,0.0f,cos };

        //前のインデックスと同じ
        data.vertices[i + 3] = data.vertices[i + 2];

        //i+1のインデックスと同じ
        data.vertices[i + 4] = data.vertices[i + 1];
        //底辺右
        data.vertices[i + 5].position = { -sinNext * bottomRadius,0.0f,cosNext * bottomRadius,1.0f };
        data.vertices[i + 5].texcoord = { uNext,1.0f };
        data.vertices[i + 5].normal = { -sinNext,0.0f,cosNext };

    }

    if (isFlip) {
        //ｙ反転させる
        for (uint32_t i = 0; i < data.vertices.size(); ++i) {
            data.vertices[i].texcoord.y = 1.0f-data.vertices[i].texcoord.y;
        }
    }

    //三角トポロジ
    data.topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    return data;
}

void Primitive::PreDraw(ID3D12GraphicsCommandList* commandList, const BlendMode& blendMode, const CullMode& cullMode) {

    commandList->SetGraphicsRootSignature(PSO::GetRootSignature()->GetRootSignature(RootSignature::NORMAL));

    if (meshType_ == MeshType::kLine) {
        //ライン用のPSO
        commandList->SetPipelineState(PSO::GetGraphicsPipelineStateLine().Get());//PSOを設定
    } else {
        //普通のPSO
        commandList->SetPipelineState(PSO::GetGraphicsPipelineState(blendMode, cullMode).Get());//PSOを設定
    }
}

void Primitive::Create(const MeshData& meshData)
{

    // dataからトポロジーを保存し、それに基づいてmeshTypeを決定する
    if (meshData.topology == D3D_PRIMITIVE_TOPOLOGY_LINELIST) {
        meshType_ = MeshType::kLine;
    } else {
        meshType_ = MeshType::kNormal;
    }
    topology_ = meshData.topology;
    vertexCount_ = static_cast<UINT>(meshData.vertices.size());
    indexCount_ = static_cast<UINT>(meshData.indices.size());

    // 1. 頂点バッファの作成とデータ転送
    UINT vertexBufferSize = sizeof(VertexData) * vertexCount_;
    vertexResource_ = DirectXCommon::CreateBufferResource(vertexBufferSize);

    //頂点バッファビューを作成する
    vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
    vertexBufferView_.SizeInBytes = vertexBufferSize;
    vertexBufferView_.StrideInBytes = sizeof(VertexData);

    VertexData* vertexMap = nullptr;

    //頂点リソースにマッピング
    vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexMap));
    std::memcpy(vertexMap, meshData.vertices.data(), vertexBufferSize); // vectorの中身を一気にコピー！
    vertexResource_->Unmap(0, nullptr);

    if (indexCount_ > 0) {
        //インデックスがある場合

        UINT indexBufferSize = sizeof(uint32_t) * indexCount_;
        indexResource_ = DirectXCommon::CreateBufferResource(indexBufferSize);
        //Viewを作成する IndexBufferView(IBV)

        //リソースの先頭アドレスから使う
        indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
        //使用するリソースのサイズ
        indexBufferView_.SizeInBytes = indexBufferSize;
        //インデックスはuint32_tとする
        indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

        //インデックスリーソースにデータを書き込む
        uint32_t* indexMap = nullptr;
        indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexMap));
        std::memcpy(indexMap, meshData.indices.data(), indexBufferSize);
        indexResource_->Unmap(0, nullptr);
    }
}

void Primitive::Draw(ID3D12GraphicsCommandList* commandList)
{
    //形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておけばよい。
    commandList->IASetPrimitiveTopology(topology_);
    commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);

    if (indexCount_ > 0) {
        commandList->IASetIndexBuffer(&indexBufferView_);
        commandList->DrawIndexedInstanced(indexCount_, 1, 0, 0, 0);
    } else {
        commandList->DrawInstanced(vertexCount_, 1, 0, 0); // ラインなどインデックスが無い場合
    }
}

void Primitive::DrawCallForParticle(ID3D12GraphicsCommandList* commandList, const uint32_t numInstance)
{
    commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
    //SrvManager::SetGraphicsRootDescriptorTable(2, textureHandle_);

    if (indexCount_ > 0) {
        commandList->IASetIndexBuffer(&indexBufferView_);
        commandList->DrawIndexedInstanced(indexCount_, numInstance, 0, 0, 0);
    } else {
        commandList->DrawInstanced(vertexCount_, numInstance, 0, 0); // ラインなどインデックスが無い場合
    }
}


