#pragma once

#include <string>

namespace model {

	struct IModelLoader {
		virtual ~IModelLoader() = default;

		virtual bool load(const std::string& _modelDir, const std::string& _modelFile, const std::string _texDir) = 0;
	};

}