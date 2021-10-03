#pragma once

#include <string>
#include <vector>
#include <variant>

namespace model {

	using VString = std::variant<std::string, std::wstring>;

	// ���f����ǂݎ������̍ŏI�`��
	struct ModelData {
	public:
		struct ModelVertex {
			float position[3];
			float normal[3];
			float uv[2];
		};

		struct Material {
			VString textureName;
			unsigned long vertCount;
		};

	public:
		VString extension;
		VString modelName;
		std::vector<ModelVertex> vertexes;
		std::vector<unsigned int> indexes;
		std::vector<Material> materials;
	};

}