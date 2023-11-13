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

		inline std::vector<std::shared_ptr<FBXGeometry>>& getFBXGeometrys() {
			return fbxGeometrys;
		}

	// private:
	public: // Todo: 単体テストのため、一時的にpublic
		void createFbxScene();

		// std::shared_ptr<FBXGlobalSetting> createFBXGlobalSetting();

		std::vector<std::shared_ptr<FBXGeometry>> createFBXGeometry();

	private:
		long long version;
		std::shared_ptr<FBXNode> rootNode;
		
		std::shared_ptr<FBXGlobalSetting> globalSetting;
		std::vector<std::shared_ptr<FBXGeometry>> fbxGeometrys;
	};

}