#pragma once

#include <string>
#include <vector>
#include <variant>
#include <unordered_map>
#include <memory>

#include "PMX/PMXDataStruct.hpp"
#include "Vertex/Vertex.hpp"

namespace model {

	struct ModelVertex {
		Vertex3 position;
		Vertex3 normal;
		Vertex2 uv;
	};

	// 描画用のモデルデータ
	struct ModelData {
	public:
		
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
		std::vector<std::vector<ModelVertex>> vertexes;
		std::vector<std::vector<unsigned int>> indexes;
		std::vector<Material> materials;
	};

}

using ModelDataPtr = std::shared_ptr<model::ModelData>;
using ModelList = std::unordered_map<std::string, ModelDataPtr>;