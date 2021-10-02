#pragma once

#include "../ModelLoader.hpp"
#include "PMXFile.hpp"

namespace model::pmx {

	class PMXLoader final : public IModelLoader {
	public:
		PMXLoader();
		~PMXLoader() = default;

		bool load(const std::string& _modelPath, const std::string _texPath) override;

	private:
		PMXFile pmxFile;
	};

}