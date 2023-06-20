#include "ModelImporter.hpp"

#include "PMD/PMDFile.hpp"
#include "PMX/PMXFile.hpp"
#include "ModelLoader.hpp"
#include "PMD/PMDLoader.hpp"
#include "PMX/PMXLoader.hpp"
#include "Utility/StringUtility.hpp"

#include <iostream>

namespace {

	// PMDの形式からModelDataの形式に変換
	void convertPMDToModelData(ModelDataPtr& _modelData, const model::pmd::PMDFile& _pmdFile) {

		_modelData->extension = "pmd";
		_modelData->modelName = su::getWString(_pmdFile.header.modelName);
		su::rep(_modelData->modelName, L"\\", L"/");

		{
			size_t vertexSize = _pmdFile.vertexes.size();
			_modelData->vertexes.resize(vertexSize);
			for (size_t i{ 0 }; i < vertexSize; ++i) {
				std::memcpy(_modelData->vertexes[i].position, _pmdFile.vertexes[i].position, sizeof(_pmdFile.vertexes[i].position));
				std::memcpy(_modelData->vertexes[i].normal, _pmdFile.vertexes[i].normal, sizeof(_pmdFile.vertexes[i].normal));
				std::memcpy(_modelData->vertexes[i].uv, _pmdFile.vertexes[i].uv, sizeof(_pmdFile.vertexes[i].uv));
			}
		}

		{
			size_t indexSize = _pmdFile.indexes.size();
			_modelData->indexes.resize(indexSize);
			std::copy(_pmdFile.indexes.begin(), _pmdFile.indexes.end(), _modelData->indexes.begin());
		}

		{
			size_t materialSize = _pmdFile.materials.size();
			_modelData->materials.resize(materialSize);
			for (size_t i{ 0 }; i < materialSize; ++i) {

				std::string textureStr{ _pmdFile.materials[i].textureFileName };
				// TODO: もう少し処理を最適化する
				if (textureStr[textureStr.size() - 1] == '/') { // パスの最後の文字が/なら、画像へのパスがないので追加する
					_modelData->materials[i].textureName = _modelData->materials[i - 1].textureName;
				}
				else {
					// スフィアマップを取り除く
					auto idx = textureStr.find('*');
					if (idx != std::string::npos) {
						std::wcout << su::getWString(textureStr.substr(idx + 1, textureStr.size())) << std::endl;
						textureStr = textureStr.substr(0, idx);
					}

					_modelData->materials[i].textureName = su::getWString(textureStr);
					su::rep(_modelData->materials[i].textureName, L"\\", L"/");
				}
				_modelData->materials[i].vertCount = _pmdFile.materials[i].faceVertCount;
			}
		}
	}

	// PMXの形式からModelDataの形式に変換
	void convertPMXToModelData(ModelDataPtr& _modelData, const model::pmx::PMXFile& _pmxFile) {
		_modelData->extension = "pmx";
		_modelData->modelName = su::getWString(_pmxFile.modelInfo.modelName);

		{
			size_t vertexSize = _pmxFile.vertexes.size();
			_modelData->vertexes.resize(vertexSize);
			for (size_t i{ 0 }; i < vertexSize; ++i) {
				std::memcpy(_modelData->vertexes[i].position, _pmxFile.vertexes[i].position, sizeof(_pmxFile.vertexes[i].position));
				std::memcpy(_modelData->vertexes[i].normal, _pmxFile.vertexes[i].normal, sizeof(_pmxFile.vertexes[i].normal));
				std::memcpy(_modelData->vertexes[i].uv, _pmxFile.vertexes[i].uv, sizeof(_pmxFile.vertexes[i].uv));
			}
		}

		{
			size_t indexSize = _pmxFile.indexes.size();
			_modelData->indexes.resize(indexSize);
			std::copy(_pmxFile.indexes.begin(), _pmxFile.indexes.end(), _modelData->indexes.begin());
		}

		const size_t MaterialSize = _pmxFile.materials.size();
		_modelData->materials.resize(MaterialSize);

		// マテリアルのセット
		{
			for (size_t i{ 0 }; i < MaterialSize; ++i) {
				auto& modelMaterial = _modelData->materials[i].modelMaterial;
				std::memcpy(modelMaterial.diffuse, _pmxFile.materials[i].diffuse, sizeof(float) * 4);
				std::memcpy(modelMaterial.specular, _pmxFile.materials[i].specular, sizeof(float) * 3);
				modelMaterial.specular[3] = _pmxFile.materials[i].specularCoefficient;
				std::memcpy(modelMaterial.ambient, _pmxFile.materials[i].ambient, sizeof(float) * 3);
			}
		}

		// テクスチャのセット
		{
			for (size_t i{ 0 }; i < MaterialSize; ++i) {
				auto textureIndex = _pmxFile.materials[i].normalTextureIndex;

				_modelData->materials[i].materialName = su::getWString(_pmxFile.materials[i].materialName);
				_modelData->materials[i].vertCount = _pmxFile.materials[i].surface;

				if (textureIndex == 0xFF) {
					// textureIndex = 0;
					continue;
				}
				_modelData->materials[i].textureName = su::getWString(_pmxFile.textures[textureIndex]);
				su::rep(_modelData->materials[i].textureName, L"\\", L"/");
			}
		}

		// スフィアテクスチャのセット
		{
			for (size_t i{ 0 }; i < MaterialSize; ++i) {
				auto sphereIndex = _pmxFile.materials[i].sphereTextureIndex;
				if(sphereIndex != 0xff){
					_modelData->materials[i].mode = _pmxFile.materials[i].mode;
					_modelData->materials[i].sphereName = su::getWString(_pmxFile.textures[sphereIndex]);
					su::rep(_modelData->materials[i].sphereName, L"\\", L"/");
				}
			}
		}

		// トゥーンテクスチャのセット
		{
			for (size_t i{ 0 }; i < MaterialSize; ++i) {
				auto toonIndex = _pmxFile.materials[i].indexSize;
				// if (toonIndex != 0xff) {
				if (toonIndex < _pmxFile.textures.size()) {
					_modelData->materials[i].toonName = su::getWString(_pmxFile.textures[toonIndex]);
					su::rep(_modelData->materials[i].toonName, L"\\", L"/");
				}
			}
		}
	}
}

ModelImporter::ModelImporter()
	: modelList()
{
}

void ModelImporter::loadModel(const std::string& _name, const ModelDesc& _modelDesc)
{
	loadModel(_name, _modelDesc.modelDirectoy, _modelDesc.modelFileName);
}

void ModelImporter::loadModel(const std::string& _name, const std::string& _modelDir, const std::string& _modelFile)
{
	std::string extension = su::getExtension(_modelFile);

	if (extension == "pmd") {
		model::pmd::PMDLoader pmdLoader{};
		pmdLoader.load(_modelDir, _modelFile);

		ModelDataPtr modelData{ new model::ModelData{} };
		convertPMDToModelData(modelData, pmdLoader.getFile());

		modelList.emplace(_name, modelData);
	}

	if (extension == "pmx") {
		model::pmx::PMXLoader pmxLoader{};
		pmxLoader.load(_modelDir, _modelFile);

		ModelDataPtr modelData{ new model::ModelData{} };
		convertPMXToModelData(modelData, pmxLoader.getFile());

		modelList.emplace(_name, modelData);
	}

}

ModelDataPtr ModelImporter::getModelData(const std::string& _name)
{
	if (!modelList.contains(_name)) {
		return nullptr;
	}

	return modelList[_name];
}

ModelDataPtr ModelImporter::getModelData(const std::string& _name, const std::string& _modelDir, const std::string& _modelFile)
{
	if (!modelList.contains(_name)) {
		loadModel(_name, _modelDir, _modelFile);
	}

	return modelList[_name];
}
