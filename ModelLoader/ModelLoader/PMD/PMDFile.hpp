#pragma once

#include <vector>
#include "PMDDataStruct.hpp"

namespace model::pmd {

	struct PMDFile {
		PMDHeader header;
		std::vector<VertexData> vertexes;
		std::vector<unsigned int> indexes; // unsigned short
		std::vector<Material> materials;
	};

}