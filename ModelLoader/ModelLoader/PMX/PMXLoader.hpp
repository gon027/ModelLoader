#pragma once

#include "../ModelLoader.hpp"
#include "../../ModelData.hpp"
#include "PMXFile.hpp"

namespace model::pmx {

	class PMXLoader final : public IModelLoader {
	public:
		PMXLoader();
		~PMXLoader() = default;

		bool load(const std::string& _modelDir, const std::string& _modelFile, const std::string _texDir) override;

		inline PMXFile& getFile() {
			return pmxFile;
		}

	private:
		PMXFile pmxFile;
		ModelData modelData;
	};

}