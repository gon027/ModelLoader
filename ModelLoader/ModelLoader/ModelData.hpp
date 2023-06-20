#pragma once

#include <string>
#include <vector>
#include <variant>
#include <unordered_map>
#include <memory>

#include "PMX/PMXDataStruct.hpp"

namespace model {

	// 描画用のモデルデータ
	struct ModelData {
	public:
		struct ModelVertex {
			float position[3];
			float normal[3];
			float uv[2];
		};

		struct ModelMaterial {
			float diffuse[4];
			float specular[4];
			float ambient[3];
		};

		struct Material {
			std::wstring materialName;
			std::wstring textureName;
			std::wstring sphereName;
			pmx::SphereMode mode;
			std::wstring toonName;
			unsigned long vertCount;
			ModelMaterial modelMaterial;
		};

	public:
		std::string extension;
		std::wstring modelName;
		std::vector<ModelVertex> vertexes;
		std::vector<unsigned int> indexes;
		std::vector<Material> materials;
	};

}

using ModelDataPtr = std::shared_ptr<model::ModelData>;
using ModelList = std::unordered_map<std::string, ModelDataPtr>;