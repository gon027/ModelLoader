#pragma once

#include "../ModelLoader.hpp"
#include "../ModelData.hpp"
#include "PMDFile.hpp"

namespace model::pmd {

	class PMDLoader final : public IModelLoader {
	public:
		PMDLoader();
		~PMDLoader() = default;

		bool load(const std::string& _modelDir, const std::string& _modelFile) override;

		inline PMDFile& getFile() {
			return pmdFile;
		}

	private:
		PMDFile pmdFile;
	};

}