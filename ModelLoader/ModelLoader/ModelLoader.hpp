#pragma once

#include <string>

namespace model {

	struct IModelLoader {
		virtual ~IModelLoader() = default;

		virtual bool load(const std::string& _modelPath, const std::string _texPath) = 0;
	};

}