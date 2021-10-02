#pragma once

#include <vector>
#include "PMXDataStruct.hpp"

namespace model::pmx {

	struct PMXFile {
		PMXHeader header;
		ModelInfo modelInfo;
		std::vector<PMXVertex> vertexes;
		std::vector<unsigned int> indexes;
		std::vector<Material> materials;
	};

}