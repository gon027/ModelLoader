#pragma once

#include <string>
#include "FBXDataStruct.hpp"
#include "../BinaryFile/BinaryFile.hpp"

namespace model::fbx {

	class FBXLoader {
	public:
		FBXLoader();
		~FBXLoader();

		bool load(const std::string& _modelDir);

	private:


	private:
		BinaryFile bf;
		long long version;

	};

}