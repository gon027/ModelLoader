#include "PMDLoader.hpp"

#include <iostream>
#include <fstream>
#include <algorithm>

namespace model::pmd {

	PMDLoader::PMDLoader()
		: pmdFile()
	{
	}

	bool PMDLoader::load(const std::string& _modelPath, const std::string _texPath)
	{

		std::ifstream ifs{ _modelPath, std::ios::in | std::ios::binary };
		if (!ifs) {
			return false;
		}

		{
			ifs.read((char*)&pmdFile.header.name, sizeof(pmdFile.header.name));
			ifs.read((char*)&pmdFile.header.version, sizeof(pmdFile.header.version));
			ifs.read((char*)&pmdFile.header.modelName, sizeof(pmdFile.header.modelName));
			ifs.read((char*)&pmdFile.header.comment, sizeof(pmdFile.header.comment));

			std::cout << pmdFile.header.name << std::endl;
			std::cout << pmdFile.header.version << std::endl;
			std::cout << pmdFile.header.modelName << std::endl;
			std::cout << pmdFile.header.comment << std::endl;
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

		return true;
	}

}