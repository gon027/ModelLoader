#pragma once

#include "../ModelLoader.hpp"
#include "PMXFile.hpp"

namespace model::pmx {

	class PMXLoader final : public IModelLoader {
	public:
		PMXLoader();
		~PMXLoader() = default;

		bool load(const std::string& _modelDir, const std::string& _modelFile, const std::string _texDir) override;

	private:
		PMXFile pmxFile;
	};

}