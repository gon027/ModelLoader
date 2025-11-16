#include "../../include/OBJ/OBJLoader.hpp"
#include "../../include/OBJ/OBJData.hpp"

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <tuple>
#include <unordered_map>

namespace model::obj {

	// https://asura.iaigiri.com/OpenGL/gl15_2.html

	namespace {

		constexpr char CHAR_BLANK{ ' ' };
		constexpr char CHAR_SLASH{ '/' };

		const std::string STR_BLANK = " ";

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

	ObjLoader::ObjLoader()
		: folderPath{}
		, isLoadFlag{ false }
	{
	}

	bool ObjLoader::load(const std::string& _folderPath, const std::string& _fileName) {

		folderPath = _folderPath;

		//  objファイルを読み込み
		if (!loadObjFile(_fileName)) {
			return false;
		}

		// mltファイルを読み込む
		if (!loadMtlFile()) {
			return false;
		}

		isLoadFlag = true;
		return true;
	}

	bool ObjLoader::isLoad()
	{
		return isLoadFlag;
	}

	bool ObjLoader::loadObjFile(const std::string& _objFileName)
	{
		std::ifstream ifs{ folderPath + _objFileName };
		if (!ifs) {
			return false;
		}

		if (!ifs.is_open()) {
			return false;
		}

		std::vector<Vertex3> vertices;
		std::vector<Vertex2> texcoords;
		std::vector<Vertex3> normals;

		std::unordered_map<std::string, int> vecVertIdxMap{};
		std::unordered_map<std::string, int> vecIndxIdxMap{};
		std::unordered_map<std::string, int> vecNormIdxMap{};

		std::string line{};
		while (true) {

			ifs >> line;
			// std::cout << line << std::endl;

			if (ifs.eof()) {
				break;
			}

			if (line == "#") {
				; // コメント行のため処理を行わない
			}

			if (line == "mtllib") {
				ifs >> mtlFileName;
			}

			if (line == "v") {
				float x{}, y{}, z{};
				ifs >> x >> y >> z;
				Vertex3 vert{ x, y, z };
				vertices.push_back(vert);
			}

			if (line == "vt") {
				float u{}, v{};
				ifs >> u >> v;
				Vertex2 texcoord{ u, v };
				texcoords.push_back(texcoord);
			}

			if (line == "vn") {
				float x{}, y{}, z{};
				ifs >> x >> y >> z;
				Vertex3 normal{ x, y, z };
				normals.push_back(normal);
			}

			if (line == "f") {
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

				const auto parseInteger{
					[](const std::string& _num, int _size) -> int {
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
					}
				};

				auto hoge{
					[&](const std::string& unit) -> std::tuple<int, int, int> {
						int vIndex{ -1 };
						int vtIndex{ -1 };
						int vnIndex{ -1 };

						std::vector<std::string> splitUnit = split(unit, CHAR_SLASH);

						// 要素が1なら[頂点]
						if (splitUnit.size() == 1) {
							// 頂点
							// vIndex = parseInteger(splitUnit[0], VERTEX_SIZE);

							if (vecVertIdxMap.contains(splitUnit[0])) {
								vIndex = vecVertIdxMap[splitUnit[0]];
							}
							else {
								vIndex = parseInteger(splitUnit[0], VERTEX_SIZE);
								vecVertIdxMap[splitUnit[0]] = vIndex;
							}
						}

						// 要素が2なら[頂点, UV]
						if (splitUnit.size() == 2) {
							// 頂点
							// vIndex = parseInteger(splitUnit[0], VERTEX_SIZE);
							if (vecVertIdxMap.contains(splitUnit[0])) {
								vIndex = vecVertIdxMap[splitUnit[0]];
							}
							else {
								vIndex = parseInteger(splitUnit[0], VERTEX_SIZE);
								vecVertIdxMap[splitUnit[0]] = vIndex;
							}

							// テクスチャ座標
							// vtIndex = parseInteger(splitUnit[1], TEXCOORD_SIZE);
							if (vecIndxIdxMap.contains(splitUnit[1])) {
								vtIndex = vecIndxIdxMap[splitUnit[1]];
							}
							else {
								vtIndex = parseInteger(splitUnit[1], TEXCOORD_SIZE);
								vecIndxIdxMap[splitUnit[1]] = vtIndex;
							}
						}

						// 要素が3なら[頂点, UV, 法線]
						// 要素が3かつ真ん中が空白の場合[頂点, , 法線]
						if (splitUnit.size() == 3) {
							// 頂点
							// vIndex = parseInteger(splitUnit[0], VERTEX_SIZE);

							if (vecVertIdxMap.contains(splitUnit[0])) {
								vIndex = vecVertIdxMap[splitUnit[0]];
							}
							else {
								vIndex = parseInteger(splitUnit[0], VERTEX_SIZE);
								vecVertIdxMap[splitUnit[0]] = vIndex;
							}

							// テクスチャ座標は、値が入っていたら変換する
							if (!splitUnit[1].empty() && splitUnit[1] != " ") {
								// vtIndex = parseInteger(splitUnit[1], TEXCOORD_SIZE);

								if (vecIndxIdxMap.contains(splitUnit[1])) {
									vtIndex = vecIndxIdxMap[splitUnit[1]];
								}
								else {
									vtIndex = parseInteger(splitUnit[1], TEXCOORD_SIZE);
									vecIndxIdxMap[splitUnit[1]] = vtIndex;
								}
							}

							// 法線
							// vnIndex = parseInteger(splitUnit[2], NORMAL_SIZE);
							if (vecNormIdxMap.contains(splitUnit[2])) {
								vnIndex = vecNormIdxMap[splitUnit[2]];
							}
							else {
								vnIndex = parseInteger(splitUnit[2], NORMAL_SIZE);
								vecNormIdxMap[splitUnit[2]] = vnIndex;
							}
						}

						return { vIndex, vtIndex, vnIndex };
					}
				};

				//using Vt = std::vector<std::tuple<int, int, int>>;
				//Vt faceVec{};

				//std::vector<std::string> sLine = split(oneLine, CHAR_BLANK);
				//for (const auto& unit : sLine) {
				//	faceVec.push_back(hoge(unit));
				//}
				//
				//int faceNum{ static_cast<int>(ceil(static_cast<double>(faceVec.size()) / 3.0)) };
				//for (int i{ 0 }; i < faceNum; ++i) {

				//	for (int idx{ i }; idx < 3 + i /*faceVec.size()*/; ++idx) {
				//		// int hogei = (idx + 1) % faceVec.size();
				//		const auto& [vIndex, vtIndex, vnIndex] = faceVec[idx];

				//		// 取得したインデックスからObjVertexを作成する
				//		ObjVertex oVertex{};

				//		// 頂点
				//		if (vIndex >= 0) {
				//			oVertex.position = vertices[vIndex];
				//		}

				//		//  テクスチャ座標
				//		if (vtIndex >= 0) {
				//			oVertex.texcoord = texcoords[vtIndex];
				//		}

				//		// 法線
				//		if (vnIndex >= 0) {
				//			oVertex.normal = normals[vnIndex];
				//		}

				//		objVertices.emplace_back(oVertex);
				//	}
				//}


				int count{ 0 };
				std::vector<std::string> sLine = split(oneLine, CHAR_BLANK);
				for (const auto& unit : sLine) {

					const auto& [vIndex, vtIndex, vnIndex] = hoge(unit);
					//std::cout << "{ " << vIndex << ", " << vtIndex << ", " << vnIndex << " }" << std::endl;

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
						int hogei = (i + 1) % 4;
						auto& unit = sLine[hogei];
						const auto& [vIndex, vtIndex, vnIndex] = hoge(unit);
						// std::cout << "{ " << vIndex << ", " << vtIndex << ", " << vnIndex << " }" << std::endl;

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

	bool ObjLoader::loadMtlFile()
	{
		if (mtlFileName == "") {
			return true;
		}

		std::ifstream ifs{ folderPath + mtlFileName };
		if (!ifs) {
			return false;
		}

		if (!ifs.is_open()) {
			return false;
		}

		std::string mtlName{ "" };
		ObjMaterial objMat{};
		bool flag = false;

		std::string line{};
		while (true) {
			ifs >> line;
			// std::cout << line << std::endl;

			if (ifs.eof()) {
				break;
			}

			if (line == "#") {
				; // コメント行のため処理を行わない
			}

			if (line == "newmtl") {
				std::cout << "newmtl" << std::endl;
				if (mtlName == "") {
					ifs >> mtlName;
					objMat = {};
					flag = true;
				}
				else {
					ifs >> mtlName;
					// 格納する
					objMaterials.emplace_back(objMat);

					// 初期化
					objMat = {};
				}
			}

			// アンビエント
			if (line == "Ka") {
				float x{}, y{}, z{};
				ifs >> x >> y >> z;
				material.ambient = { x, y, z };

				objMat.ambient = { x, y, z };
			}

			// ディフューズ 
			if (line == "Kd") {
				float x{}, y{}, z{};
				ifs >> x >> y >> z;
				material.diffuse = { x, y, z };

				objMat.diffuse = { x, y, z };
			}

			// スペキュラ
			if (line == "Ks") {
				float x{}, y{}, z{};
				ifs >> x >> y >> z;
				material.specula = { x, y, z };

				objMat.specula = { x, y, z };
			}

			// スペキュラ指数
			if (line == "Ns") {
				float value{ 0.0f };
				ifs >> value;
				material.speculaWeight = value;

				objMat.speculaWeight = value;
			}

			//  ディゾルブ
			if (line == "d") {
				float value{ 0.0f };
				ifs >> value;
				material.dissolve = value;

				objMat.dissolve = value;
			}

			// 屈折率
			if (line == "Ni") {
				float value{ 0.0f };
				ifs >> value;
				material.refractive = value;

				objMat.refractive = value;
			}

			// ディフューズテクスチャマップ
			if (line == "map_Kd") {
				std::string name;
				ifs >> name;
				material.kdTextureName = name;

				objMat.kdTextureName = name;
			}
		}

		if (flag) {
			// 格納する
			objMaterials.emplace_back(objMat);
		}

		return true;
	}
}