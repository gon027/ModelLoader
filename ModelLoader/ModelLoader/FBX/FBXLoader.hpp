#pragma once

#include <string>
#include <unordered_map>
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

		inline std::unordered_map<long long, std::shared_ptr<FBXGeometry>>& getFBXGeometrys() {
			return fbxGeometrys;
		}

		inline std::unordered_map<long long, FBXModel>& getFBXModels() {
			return fbxModels;
		}

		inline std::unordered_map<long long, FBXMaterial>& getFBXMaterials() {
			return fbxMaterial;
		}

		inline std::unordered_map<long long, FBXTexture>& getFBXTextures() {
			return fbxTextures;
		}

		inline std::unordered_multimap<long long, long long>& getOO() {
			return oos;
		}

		inline std::unordered_multimap<long long, long long>& getOP() {
			return ops;
		}

	// private:
	public: // Todo: 単体テストのため、一時的にpublic
		void createFbxScene();

		void createFbxTexture();

		void createConnections();

		void createMaterial();

		void createModel();

		// std::shared_ptr<FBXGlobalSetting> createFBXGlobalSetting();

		void createFBXGeometry();

	private:
		long long version;
		std::shared_ptr<FBXNode> rootNode;
		
		std::shared_ptr<FBXGlobalSetting> globalSetting;
		std::unordered_map<long long, std::shared_ptr<FBXGeometry>> fbxGeometrys;
		std::unordered_map<long long, FBXTexture> fbxTextures;
		std::unordered_map<long long, FBXMaterial> fbxMaterial;
		std::unordered_map<long long, FBXModel> fbxModels;

		std::unordered_multimap<long long, long long> ops;
		std::unordered_multimap<long long, long long> oos;

	};

}