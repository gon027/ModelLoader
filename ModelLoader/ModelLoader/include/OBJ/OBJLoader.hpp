#pragma once

#include <string>
#include <vector>
#include "OBJData.hpp"

namespace model::obj {

	class ObjLoader {
	public:
		ObjLoader();
		~ObjLoader() = default;

		bool load(const std::string& _folderPath, const std::string& _fileName);

		inline std::vector<ObjVertex>& getObjVertex() {
			return objVertices;
		}

		inline ObjMaterial& getMaterial() {
			return material;
		}

	private:
		bool loadObjFile(const std::string& _objFileName);
		bool loadMtlFile();

	private:
		std::string folderPath;

		// 最終的に出力する頂点
		std::vector<ObjVertex> objVertices;

		// マテリアル
		ObjMaterial material;

		std::vector<ObjMaterial> objMaterials;

		std::string mtlFileName;
	};

}