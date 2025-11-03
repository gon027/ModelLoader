#pragma once

#include <string>
#include <vector>
#include "OBJData.hpp"

namespace model::obj {

	class ObjLoader {
	public:
		ObjLoader() = default;
		~ObjLoader() = default;

		bool load(const std::string& _fileName);

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
		bool loadObjFile();
		bool loadMtlFile();

	private:
		std::vector<Vertex3> vertices;
		std::vector<Vertex2> texcoords;
		std::vector<Vertex3> normals;

		// 最終的に出力する頂点
		std::vector<ObjVertex> objVertices;

		std::string mtlFileName;
	};

}