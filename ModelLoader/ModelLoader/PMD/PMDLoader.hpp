#pragma once

#include "../ModelLoader.hpp"
#include "PMDFile.hpp"

namespace model::pmd {

	class PMDLoader final : public IModelLoader {
	public:
		PMDLoader();
		~PMDLoader() = default;

		bool load(const std::string& _modelDir, const std::string& _modelFile, const std::string _texDir) override;

	private:
		PMDFile pmdFile;
	};

}