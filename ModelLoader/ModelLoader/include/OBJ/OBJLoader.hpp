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

		inline std::vector<Vertex3>& getVertex() {
			return vertices;
		}

		inline std::vector<Vertex2>& getTexcoord() {
			return texcoords;
		}

		inline std::vector<Vertex3>& getNormal() {
			return normals;
		}

		inline std::vector<ObjVertex>& getObjVertex() {
			return objVertices;
		}

	private:
		bool loadObjFile(const std::string& _objFileName);
		bool loadMtlFile();

	private:
		std::string folderPath;

		std::vector<Vertex3> vertices;
		std::vector<Vertex2> texcoords;
		std::vector<Vertex3> normals;

		// 最終的に出力する頂点
		std::vector<ObjVertex> objVertices;

		// マテリアル
		ObjMaterial material;

		std::string mtlFileName;
	};

}