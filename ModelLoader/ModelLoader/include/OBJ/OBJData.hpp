#pragma once

#include "../Vertex/Vertex.hpp"

namespace model::obj {

	// https://asura.iaigiri.com/OpenGL/gl15_2.html

	// 頂点(v)
	// 法線(vn)
	// 面情報
	// 材質

	struct ObjVertex {
		model::Vertex3 position;
		model::Vertex2 texcoord;
		model::Vertex3 normal;

	public:
		ObjVertex()
			: position{}, texcoord{}, normal{}
		{}
	};

	struct ObjMaterial {
		std::string materialName;
		Vertex3 ambient;      // Ka: アンビエント
		Vertex3 diffuse;      // Kd: ディフューズ
		Vertex3 specula;      // Ks: スペキュラ
		float speculaWeight;  // Ns: スペキュラ指数(重み)
		float dissolve;       // d: ディゾルブ(透過)
		float refractive;     // Ni: 光学密度(屈折率) [0.0001, 10]
		// int model; // 照射モデル
		std::string kdTextureName;  // map_Kd: ディフューズテクスチャマップ

	public:
		ObjMaterial() = default;
	};
}