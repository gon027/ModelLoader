#pragma once

#include "../ModelLoader.hpp"
#include "PMDFile.hpp"

namespace model::pmd {

	class PMDLoader final : public IModelLoader {
	public:
		PMDLoader();
		~PMDLoader() = default;

		bool load(const std::string& _modelPath, const std::string _texPath);

	private:
		PMDFile pmdFile;
	};

}