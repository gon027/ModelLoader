#include "../../include/OBJ/OBJLoader.hpp"
#include "../../include/OBJ/OBJData.hpp"

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

namespace model::obj {

	// https://asura.iaigiri.com/OpenGL/gl15_2.html
	// https://ja.wikipedia.org/wiki/Wavefront_.obj%E3%83%95%E3%82%A1%E3%82%A4%E3%83%AB

	namespace {

		constexpr char CHAR_BLANK{ ' ' };
		constexpr char CHAR_SLASH{ '/' };

		const std::string STR_BLANK = " ";

		/// <summary>
		/// 空白の侵入を許さない
		/// </summary>
		/// <param name="s"></param>
		/// <param name="delim"></param>
		/// <returns></returns>
		std::vector<std::string> split1(const std::string& s, char delim) {
			std::vector<std::string> result{};

			std::stringstream ss{ s };
			std::string item{};
			while (std::getline(ss, item, delim)) {
				if (!item.empty()) {
					result.push_back(item);
				}
			}
			return result;
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="s"></param>
		/// <param name="delim"></param>
		/// <returns></returns>
		std::vector<std::string> split(const std::string& s, char delim) {
			std::vector<std::string> result{};

			std::stringstream ss{ s };
			std::string item{};
			while (std::getline(ss, item, delim)) {
				if (!item.empty()) {
					result.push_back(item);
				}
				else {
					result.push_back(" ");
				}
			}
			return result;
		}
	}

	bool ObjLoader::load(const std::string& _fileName) {
		
		std::ifstream ifs{ _fileName };
		if (!ifs) {
			return false;
		}

		if (!ifs.is_open()) {
			return false;
		}

		std::vector<ObjVertex> t_vertex{};
		std::vector<unsigned int> t_indices{};

		std::string line;
		while (true) {

			ifs >> line;
			// std::cout << line << std::endl;

			if (ifs.eof()) {
				break;
			}

			if (line == "#") {
				;
			}
			else if (line == "mtllib") {
				ifs >> mtlFileName;
			}
			else if (line == "v") {
				float x{}, y{}, z{};
				ifs >> x >> y >> z;
				Vertex3 vert{ x, y, z };
				vertices.push_back(vert);
			}
			else if (line == "vt") {
				float u{}, v{};
				ifs >> u >> v;
				Vertex2 texcoord{ u, v };
				texcoords.push_back(texcoord);
			}
			else if (line == "vn") {
				float x{}, y{}, z{};
				ifs >> x >> y >> z;
				Vertex3 normal{ x, y, z };
				normals.push_back(normal);
			}
			else if (line == "f") {
				/*
				* 参考資料
				* https://ja.wikipedia.org/wiki/Wavefront_.obj%E3%83%95%E3%82%A1%E3%82%A4%E3%83%AB
				* 
				* 頂点インデックス
				* f v1 v2 v3
				* 
				* 頂点テクスチャ座標インデックス
				* f v1/vt1 v2/vt2 v3/vt3 ...
				* 
				* 頂点法線インデックス
				* f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3 ...
				* 
				* テクスチャ座標インデックスなし頂点法線インデックス
				* f v1//vn1 v2//vn1 v3//vn1 ...
				*/

				// インデックスを保存するVector
				const int VEC_MAX_SIZE{ 4 };
				std::vector<int> 
					vertIdxVec(VEC_MAX_SIZE, -1), 
					tecIdxVec(VEC_MAX_SIZE, -1), 
					norIdxVec(VEC_MAX_SIZE, -1);

				const int VERTEX_SIZE{ static_cast<int>(vertices.size()) };
				const int TEXCOORD_SIZE{ static_cast<int>(texcoords.size()) };
				const int NORMAL_SIZE{ static_cast<int>(normals.size()) };

				// 文字列の1文字目が空白の場合省きたい
				// fを読み取ったあと空白と前提にしている
				ifs.ignore();

				// 1行読み取る
				std::string oneLine{};
				std::getline(ifs, oneLine);

				const auto parseInteger = [](const std::string& _num, int _size) -> int {
					try {
						int parse = std::stoi(_num);
						int tmpIdx = parse;
						if (tmpIdx < 0) tmpIdx = static_cast<int>(_size) + tmpIdx;
						else tmpIdx = tmpIdx - 1;
						return tmpIdx;
					}
					catch (const std::exception& _e) {
						std::cout << "エラーログ" << std::endl;
						return -1;
					}
				};

				int count{ 0 };
				std::vector<std::string> sLine = split(oneLine, CHAR_BLANK);
				for (const auto& unit : sLine) {
					int vIndex{ -1 };
					int vtIndex{ -1 };
					int vnIndex{ -1 };

					std::vector<std::string> splitUnit = split(unit, CHAR_SLASH);

					// 要素が1なら[頂点]
					if (splitUnit.size() == 1) {
						// 頂点
						vIndex = parseInteger(splitUnit[0], VERTEX_SIZE);
					}

					// 要素が2なら[頂点, UV]
					if (splitUnit.size() == 2) {
						// 頂点
						vIndex = parseInteger(splitUnit[0], VERTEX_SIZE);

						// テクスチャ座標
						vtIndex = parseInteger(splitUnit[1], TEXCOORD_SIZE);
					}
					
					// 要素が3なら[頂点, UV, 法線]
					// 要素が3かつ真ん中が空白の場合[頂点, , 法線]
					if (splitUnit.size() == 3) {
						// 頂点
						vIndex = parseInteger(splitUnit[0], VERTEX_SIZE);

						// テクスチャ座標は、値が入っていたら変換する
						if (!splitUnit[1].empty()) vtIndex = parseInteger(splitUnit[1], TEXCOORD_SIZE);

						// 法線
						vnIndex = parseInteger(splitUnit[2], NORMAL_SIZE);
					}

					std::cout << "{ " << vIndex << ", " << vtIndex << ", " << vnIndex << " }" << std::endl;

					// 取得したインデックスからObjVertexを作成する
					ObjVertex oVertex{};

					// 頂点
					if (vIndex >= 0) {
						oVertex.position = vertices[vIndex];
					}

					//  テクスチャ座標
					if (vtIndex >= 0) {
						oVertex.texcoord = texcoords[vtIndex];
					}

					// 法線
					if (vnIndex >= 0) {
						oVertex.normal = normals[vnIndex];
					}

					objVertices.emplace_back(oVertex);
					++count;
					if (count >= 3) {
						break;
					}
				}
				
				// countの数によって処理を分ける
				// count = 4の場合、4角形
				if (sLine.size() >= 4) {
					for (int i{ 1 }; i < sLine.size(); ++i) {
						int vIndex{ -1 };
						int vtIndex{ -1 };
						int vnIndex{ -1 };

						int hoge = (i + 1) % 4;
						auto& unit = sLine[hoge];
						std::vector<std::string> splitUnit = split(unit, CHAR_SLASH);

						// 要素が1なら[頂点]
						if (splitUnit.size() == 1) {
							// 頂点
							vIndex = parseInteger(splitUnit[0], VERTEX_SIZE);
						}

						// 要素が2なら[頂点, UV]
						if (splitUnit.size() == 2) {
							// 頂点
							vIndex = parseInteger(splitUnit[0], VERTEX_SIZE);

							// テクスチャ座標
							vtIndex = parseInteger(splitUnit[1], TEXCOORD_SIZE);
						}

						// 要素が3なら[頂点, UV, 法線]
						// 要素が3かつ真ん中が空白の場合[頂点, , 法線]
						if (splitUnit.size() == 3) {
							// 頂点
							vIndex = parseInteger(splitUnit[0], VERTEX_SIZE);

							// テクスチャ座標は、値が入っていたら変換する
							if (!splitUnit[1].empty()) vtIndex = parseInteger(splitUnit[1], TEXCOORD_SIZE);

							// 法線
							vnIndex = parseInteger(splitUnit[2], NORMAL_SIZE);
						}

						std::cout << "{ " << vIndex << ", " << vtIndex << ", " << vnIndex << " }" << std::endl;

						// 取得したインデックスからObjVertexを作成する
						ObjVertex oVertex{};

						// 頂点
						if (vIndex >= 0) {
							oVertex.position = vertices[vIndex];
						}

						//  テクスチャ座標
						if (vtIndex >= 0) {
							oVertex.texcoord = texcoords[vtIndex];
						}

						// 法線
						if (vnIndex >= 0) {
							oVertex.normal = normals[vnIndex];
						}

						objVertices.emplace_back(oVertex);
					}
				}
			}
		}

		return true;
	}
}