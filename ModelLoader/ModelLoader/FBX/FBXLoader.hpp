#pragma once

#include <string>
#include "FBXDataStruct.hpp"

namespace model::fbx {

	class FBXLoader {
	public:
		FBXLoader();
		~FBXLoader();

		bool load(const std::string& _modelDir);

		inline std::shared_ptr<FBXNode> getNode() const {
			return rootNode;
		}

	private:
		void createFbxScene();

	private:
		long long version;
		std::shared_ptr<FBXNode> rootNode;
	};

}