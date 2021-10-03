#include "../PMDLoader.hpp"

#include <iostream>
#include <fstream>
#include <algorithm>

namespace model::pmd {

	namespace {

		// PMDの形式からModelDataの形式に変換
		void convertModelData(model::ModelData& _modelData, const pmd::PMDFile& _pmdFile) {

			_modelData.extension = "pmd";
			_modelData.modelName = _pmdFile.header.modelName;

			{
				size_t vertexSize = _pmdFile.vertexes.size();
				_modelData.vertexes.resize(vertexSize);
				for (size_t i{ 0 }; i < vertexSize; ++i) {
					std::memcpy(_modelData.vertexes[i].position, _pmdFile.vertexes[i].position, sizeof(_pmdFile.vertexes[i].position));
					std::memcpy(_modelData.vertexes[i].normal, _pmdFile.vertexes[i].normal, sizeof(_pmdFile.vertexes[i].normal));
					std::memcpy(_modelData.vertexes[i].uv, _pmdFile.vertexes[i].uv, sizeof(_pmdFile.vertexes[i].uv));
				}
			}

			{
				size_t indexSize = _pmdFile.indexes.size();
				_modelData.indexes.resize(indexSize);
				std::copy(_pmdFile.indexes.begin(), _pmdFile.indexes.end(), _modelData.indexes.begin());
			}

			{
				size_t materialSize = _pmdFile.materials.size();
				_modelData.materials.resize(materialSize);
				for (size_t i{ 0 }; i < materialSize; ++i) {

					std::string textureStr{ _pmdFile.materials[i].textureFileName };
					if (textureStr == "") {
						_modelData.materials[i].textureName = _modelData.materials[i - 1].textureName;
					}
					else {
						// スフィアマップを取り除く
						auto idx = textureStr.find('*');
						if (idx != std::string::npos) {
							textureStr = textureStr.substr(0, idx);
						}

						_modelData.materials[i].textureName = textureStr;
					}
					_modelData.materials[i].vertCount = _pmdFile.materials[i].faceVertCount;
				}
			}
		}

	}

	PMDLoader::PMDLoader()
		: modelData()
	{
	}

	bool PMDLoader::load(const std::string& _modelDir, const std::string& _modelFile, const std::string _texDir)
	{

		std::ifstream ifs{ _modelDir + _modelFile, std::ios::in | std::ios::binary };
		if (!ifs) {
			return false;
		}

		PMDFile pmdFile{};
		{
			ifs.read((char*)&pmdFile.header.name, sizeof(pmdFile.header.name));
			ifs.read((char*)&pmdFile.header.version, sizeof(pmdFile.header.version));
			ifs.read((char*)&pmdFile.header.modelName, sizeof(pmdFile.header.modelName));
			ifs.read((char*)&pmdFile.header.comment, sizeof(pmdFile.header.comment));

			// std::cout << pmdFile.header.name << std::endl;
			// std::cout << pmdFile.header.version << std::endl;
			// std::cout << pmdFile.header.modelName << std::endl;
			// std::cout << pmdFile.header.comment << std::endl;
		}

		{
			unsigned long vertexSize;
			ifs.read((char*)&vertexSize, sizeof(vertexSize));

			pmdFile.vertexes.resize(vertexSize);
			auto& pmdVertexes = pmdFile.vertexes;
			for (size_t i{ 0 }; i < vertexSize; ++i) {
				ifs.read((char*)&pmdVertexes[i].position,   sizeof(pmdVertexes[i].position));
				ifs.read((char*)&pmdVertexes[i].normal,     sizeof(pmdVertexes[i].normal));
				ifs.read((char*)&pmdVertexes[i].uv,         sizeof(pmdVertexes[i].uv));
				ifs.read((char*)&pmdVertexes[i].boneNum,    sizeof(pmdVertexes[i].boneNum));
				ifs.read((char*)&pmdVertexes[i].boneWeight, sizeof(pmdVertexes[i].boneWeight));
				ifs.read((char*)&pmdVertexes[i].edge,       sizeof(pmdVertexes[i].edge));
			}
		}

		{
			unsigned long indexSize;
			ifs.read((char*)&indexSize, sizeof(indexSize));

			pmdFile.indexes.resize(indexSize);
			auto& pmdIndexes = pmdFile.indexes;
			for (size_t i{ 0 }; i < indexSize; ++i) {
				ifs.read((char*)&pmdIndexes[i], sizeof(unsigned short));
			}
		}

		{
			unsigned long materialSize;
			ifs.read((char*)&materialSize, sizeof(materialSize));

			pmdFile.materials.resize(materialSize);
			auto& pmdMaterials = pmdFile.materials;
			for (size_t i{ 0 }; i < materialSize; ++i) {
				ifs.read((char*)&pmdMaterials[i].diffuseColor,    sizeof(pmdMaterials[i].diffuseColor));
				ifs.read((char*)&pmdMaterials[i].alpha,           sizeof(pmdMaterials[i].alpha));
				ifs.read((char*)&pmdMaterials[i].specularity,     sizeof(pmdMaterials[i].specularity));
				ifs.read((char*)&pmdMaterials[i].specularColor,   sizeof(pmdMaterials[i].specularColor));
				ifs.read((char*)&pmdMaterials[i].mirrorColor,     sizeof(pmdMaterials[i].mirrorColor));
				ifs.read((char*)&pmdMaterials[i].toonIndex,       sizeof(pmdMaterials[i].toonIndex));
				ifs.read((char*)&pmdMaterials[i].edgeFlag,        sizeof(pmdMaterials[i].edgeFlag));
				ifs.read((char*)&pmdMaterials[i].faceVertCount,   sizeof(pmdMaterials[i].faceVertCount));
				ifs.read((char*)&pmdMaterials[i].textureFileName, sizeof(pmdMaterials[i].textureFileName));
			}
		}

		// モデルデータを統一するための変換
		convertModelData(modelData, pmdFile);

		return true;
	}

}