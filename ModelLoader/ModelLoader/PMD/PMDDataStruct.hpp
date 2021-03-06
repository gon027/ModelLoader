#pragma once

#include <vector>
#include <string>
#include <array>

namespace model::pmd {

    /// <summary>
    /// ヘッダー
    /// </summary>
    struct PMDHeader {
        char name[3];
        float version;
        char modelName[20];
        char comment[256];
    };

    /// <summary>
    /// 頂点データ
    /// </summary>
    struct VertexData {
        float position[3];
        float normal[3];
        float uv[2];
        unsigned short boneNum[2];
        unsigned char boneWeight;
        unsigned char edge;
    };

    /// <summary>
    /// マテリアル
    /// </summary>
    struct Material {
        float diffuseColor[3];
        float alpha;
        float specularity;
        float specularColor[3];
        float mirrorColor[3];
        unsigned char toonIndex;
        unsigned char edgeFlag;
        unsigned long faceVertCount;
        std::string textureFileName;
    };

}