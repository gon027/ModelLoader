#include "ModelImporter.hpp"

#include "PMD/PMDFile.hpp"
#include "PMX/PMXFile.hpp"
#include "ModelLoader.hpp"
#include "PMD/PMDLoader.hpp"
#include "PMX/PMXLoader.hpp"
#include "Utility/StringUtility.hpp"
#include "FBX/FBXLoader.hpp"
#include "Vertex/Vertex.hpp"

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <filesystem>

namespace {

	// PMDの形式からModelDataの形式に変換
	void convertPMDToModelData(ModelDataPtr& _modelData, const model::pmd::PMDFile& _pmdFile) {

		_modelData->extension = "pmd";
		_modelData->modelName = util::getWString(_pmdFile.header.modelName);
		util::rep(_modelData->modelName, L"\\", L"/");

		// クラスの変更のため、一時的コメントアウト
		/*
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
						std::wcout << util::getWString(textureStr.substr(idx + 1, textureStr.size())) << std::endl;
						textureStr = textureStr.substr(0, idx);
					}

					_modelData->materials[i].textureName = util::getWString(textureStr);
					util::rep(_modelData->materials[i].textureName, L"\\", L"/");
				}
				_modelData->materials[i].vertCount = _pmdFile.materials[i].faceVertCount;
			}
		}
		*/
	}

	// PMXの形式からModelDataの形式に変換
	void convertPMXToModelData(ModelDataPtr& _modelData, const model::pmx::PMXFile& _pmxFile) {
		_modelData->extension = "pmx";
		_modelData->modelName = util::getWString(_pmxFile.modelInfo.modelName);

		// クラスの変更のため、一時的コメントアウト
		/*
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

				_modelData->materials[i].materialName = util::getWString(_pmxFile.materials[i].materialName);
				_modelData->materials[i].vertCount = _pmxFile.materials[i].surface;

				if (textureIndex == 0xFF) {
					// textureIndex = 0;
					continue;
				}
				_modelData->materials[i].textureName = util::getWString(_pmxFile.textures[textureIndex]);
				util::rep(_modelData->materials[i].textureName, L"\\", L"/");
			}
		}

		// スフィアテクスチャのセット
		{
			for (size_t i{ 0 }; i < MaterialSize; ++i) {
				auto sphereIndex = _pmxFile.materials[i].sphereTextureIndex;
				if(sphereIndex != 0xff){
					_modelData->materials[i].mode = _pmxFile.materials[i].mode;
					_modelData->materials[i].sphereName = util::getWString(_pmxFile.textures[sphereIndex]);
					util::rep(_modelData->materials[i].sphereName, L"\\", L"/");
				}
			}
		}

		// トゥーンテクスチャのセット
		{
			for (size_t i{ 0 }; i < MaterialSize; ++i) {
				auto toonIndex = _pmxFile.materials[i].indexSize;
				// if (toonIndex != 0xff) {
				if (toonIndex < _pmxFile.textures.size()) {
					_modelData->materials[i].toonName = util::getWString(_pmxFile.textures[toonIndex]);
					util::rep(_modelData->materials[i].toonName, L"\\", L"/");
				}
			}
		}
		*/
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
	std::string extension = util::getExtension(_modelFile);

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

float clamp(float _val, float _min, float _max) {
	return std::max(std::min(_val, _max), _min);
}

float comv(float _val) {
	if (_val < 0) return _val * -1;
	return _val;
}

void ModelImporter::loadFBX(const std::string& _name, const ModelDesc& _modelDesc)
{
	const std::filesystem::path dir{ _modelDesc.modelFileName };
	const auto parentDir = dir.parent_path();

	model::fbx::FBXLoader fbxLoader{};
	if (!fbxLoader.load(_modelDesc.modelFileName)) {
		return;
	}
	ModelDataPtr modelData{ new model::ModelData{} };

	auto& fbxGeometrys = fbxLoader.getFBXGeometrys();
	modelData->materials.resize(fbxGeometrys.size());
	size_t prevIndexBeginCount = 0;

	auto& fbxModels = fbxLoader.getFBXModels();
	auto& fbxMaterials = fbxLoader.getFBXMaterials();
	auto& fbxTextures = fbxLoader.getFBXTextures();
	auto& fbxOO = fbxLoader.getOO();
	auto& fbxOP = fbxLoader.getOP();

	std::unordered_map<long long, long long> modelMaterialIds{};
	std::unordered_multimap<long long, long long> materialTextureIds{};

	std::vector<long long> connectMaterialIds{};
	for (const auto& fbxTexture : fbxTextures) {
		const auto materialId = fbxOP.find(fbxTexture.first);
		connectMaterialIds.push_back(materialId->second);
		materialTextureIds.insert({ materialId->second, fbxTexture.first });
	}

	std::vector<long long> connectModelIds{};
	for (const auto& connectMaterialId : connectMaterialIds) {
		auto a = fbxOO.equal_range(connectMaterialId);

		std::for_each(a.first, a.second, [&](auto& e) {
			// std::cout << e.first << " : " << e.second << std::endl;

			if (fbxModels.contains(e.second)) {
				connectModelIds.push_back(e.second);
				modelMaterialIds.insert({ e.second, connectMaterialId });
			}
		});
	}

	size_t idx{ 0 };
	for (auto& fbxGeometry : fbxGeometrys) {
		auto& geometry = fbxGeometry.second;

		// index
		size_t indexSize = geometry->indexes.size();
		std::vector<unsigned int> modelIndex(indexSize);
		std::copy(geometry->indexes.begin(), geometry->indexes.end(), modelIndex.begin());
		modelData->indexes.push_back(modelIndex);
		modelData->materials[idx].vertCount = static_cast<unsigned long>(indexSize);

		// vertex
		/*
		size_t vertexSize = geometry->vertices.size();
		std::vector<model::ModelVertex> modelVertex(vertexSize);
		for (size_t i{ 0 }; i < geometry->vertices.size(); ++i) {
			modelVertex[i].position = geometry->vertices[i];
		}
		*/

		const size_t vertexSize = indexSize;
		std::vector<model::ModelVertex> modelVertex(vertexSize);
		for (size_t i{ 0 }; i < vertexSize; ++i) {
			const auto indexPointer = geometry->indexes[i];
			modelVertex[i].position = geometry->vertices[indexPointer];
		}

		// material

		// normal
		if (geometry->vertices.size() == geometry->normals.size()) {
			const size_t normalSize = indexSize;
			for (size_t i{ 0 }; i < normalSize; ++i) {
				const auto indexPointer = geometry->indexes[i];
				modelVertex[i].normal = geometry->normals[indexPointer];
				
			}
		}
		else if(geometry->indexes.size() == geometry->normals.size()) {
			const size_t normalSize = indexSize;
			for (size_t i{ 0 }; i < normalSize; ++i) {
				modelVertex[i].normal = geometry->normals[i];
			}
		}

		/*
		// uv
		if (geometry->vertices.size() == geometry->uvs.size()) {
			for (size_t i{ 0 }; i < geometry->indexes.size(); ++i) {
				const auto index = geometry->indexes[i];
				modelVertex[index].uv = geometry->uvs[index];
			}
		}
		else if (geometry->indexes.size() == geometry->uvs.size()) {
			const size_t uvSize = indexSize;
			for (size_t i{ 0 }; i < uvSize; ++i) {
				modelVertex[i].uv = geometry->uvs[i];
			}

		}
		*/

		auto connectModelId = fbxOO.find(fbxGeometry.first);  // GeometryからModelを取得
		// Geometryから取得したModelとMaterialから取得したModelに含まれているか検索
		auto connectMaterialId 
			= std::find(connectModelIds.begin(), connectModelIds.end(), connectModelId->second);
		if (connectMaterialId != connectModelIds.end()) {
			const auto id = *connectMaterialId;
			const auto materialId = modelMaterialIds[id];  // Material取得

			auto aaa = materialTextureIds.find(materialId);

			if (fbxTextures.contains(aaa->second)) {
				modelData->materials[idx].textureName 
					= parentDir.generic_wstring() + L"/" + fbxTextures[aaa->second].fileName;
			}
			
		}

		modelData->vertexes.push_back(modelVertex);

		++idx;
	}

	// transform
	modelData->transform.resize(fbxModels.size());
	idx = 0;
	for (auto& fbxModel : fbxModels) {
		auto& model = fbxModel.second;
		modelData->transform[idx].transform.x = model.lclTranslation[0];
		modelData->transform[idx].transform.y = model.lclTranslation[1];
		modelData->transform[idx].transform.z = model.lclTranslation[2];

		modelData->transform[idx].rotation.x = model.lclRotation[0];
		modelData->transform[idx].rotation.y = model.lclRotation[1];
		modelData->transform[idx].rotation.z = model.lclRotation[2];

		modelData->transform[idx].scale.x = model.lclScaling[0];
		modelData->transform[idx].scale.y = model.lclScaling[1];
		modelData->transform[idx].scale.z = model.lclScaling[2];

		++idx;
	}

	modelList.emplace(_name, modelData);
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
