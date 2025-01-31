#include "ModelImporter.hpp"

#include "include/PMD/PMDFile.hpp"
#include "include/PMX/PMXFile.hpp"
#include "include/ModelLoader.hpp"
#include "include/PMD/PMDLoader.hpp"
#include "include/PMX/PMXLoader.hpp"
#include "include/Utility/StringUtility.hpp"
#include "include/FBX/FBXLoader.hpp"
#include "include/Vertex/Vertex.hpp"

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <filesystem>

namespace {

	model::Vertex3 toVertex3(const float _origin[3]) {
		return { _origin[0], _origin[1], _origin[2] };
	}

	model::Vertex2 toVertex2(const float _origin[2]) {
		return { _origin[0], _origin[1] };
	}

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
		{
			const size_t vertexSize = _pmxFile.vertexes.size();
			_modelData->vertexes.resize(1);
			for (size_t i{ 0 }; i < 1; ++i) {
				_modelData->vertexes[i].resize(vertexSize);
				auto& vertex = _modelData->vertexes[i];
				for (size_t idx{ 0 }; idx < vertexSize; ++idx) {
					vertex[idx].position = toVertex3(_pmxFile.vertexes[idx].position);
					vertex[idx].normal = toVertex3(_pmxFile.vertexes[idx].normal);
					vertex[idx].uv = toVertex2(_pmxFile.vertexes[idx].uv);
				}
			}
		}
		
		{
			const size_t indexSize = _pmxFile.indexes.size();
			_modelData->indexes.resize(1);
			for (size_t i{ 0 }; i < 1; ++i) {
				_modelData->indexes[i].resize(indexSize);
				auto& indexes = _modelData->indexes[i];
				for (size_t idx{ 0 }; idx < indexSize; ++idx) {
					indexes[idx] = _pmxFile.indexes[idx];
				}
			}
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
	}
}

ModelImporter::ModelImporter()
	: modelList()
{
}

void ModelImporter::loadPMX(const std::string& _name, const ModelDesc& _modelDesc)
{
	const std::string extension = util::getExtension(_modelDesc.modelFileName);
	if (extension != "pmx") {
		return;
	}

	model::pmx::PMXLoader pmxLoader{};
	if (!pmxLoader.load(_modelDesc.modelDirectoy, _modelDesc.modelFileName)) {
		return;
	}

	ModelDataPtr modelData{ new model::ModelData{} };
	convertPMXToModelData(modelData, pmxLoader.getFile());

	modelList.emplace(_name, modelData);
}

void ModelImporter::loadPMD(const std::string& _name, const ModelDesc& _modelDesc)
{
	const std::string extension = util::getExtension(_modelDesc.modelFileName);
	if (extension != "pmd") {
		return;
	}

	model::pmd::PMDLoader pmdLoader{};
	if (!pmdLoader.load(_modelDesc.modelDirectoy, _modelDesc.modelFileName)) {
		return;
	}

	ModelDataPtr modelData{ new model::ModelData{} };
	convertPMDToModelData(modelData, pmdLoader.getFile());

	modelList.emplace(_name, modelData);
}

void ModelImporter::loadFBX(const std::string& _name, const ModelDesc& _modelDesc)
{
	const std::filesystem::path dir{ _modelDesc.modelFileName };
	const auto parentDir = dir.parent_path();

	model::fbx::FBXLoader fbxLoader{};
	if (!fbxLoader.load(_modelDesc.modelDirectoy + _modelDesc.modelFileName)) {
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

	std::cout << "---- Material Texture ----" << std::endl;
	std::unordered_multimap<unsigned long long, unsigned long long> materialTextureIds{};
	for (const auto& material : fbxMaterials) {
		const auto materialID = material.first;

		std::cout << "Material ID = " << materialID << std::endl;
		const auto& propertyList = fbxOP.equal_range(materialID);
		std::for_each(propertyList.first, propertyList.second, [&](const auto& _id) {
			std::cout << "    first = " << _id.first << ", second = " << _id.second << std::endl;

			const auto textureID = fbxTextures.find(_id.second);
			std::wcout << "      texture = " << textureID->second.fileName << std::endl;
			materialTextureIds.insert({ materialID, _id.second });
		});

		std::cout << std::endl;
	}

	std::cout << "---- Model Material ----" << std::endl;
	std::unordered_multimap<unsigned long long, unsigned long long> modelIds{};

	using ModelMap = std::unordered_map<unsigned long long, std::vector<unsigned long long>>;
	ModelMap modelMap{};
	for (const auto& model : fbxModels) {
		const auto modelID = model.first;
		std::cout << "Model ID = " << modelID << std::endl;
		const auto& materialObjects = fbxOO.equal_range(modelID);

		unsigned long long geometryID{};
		std::vector<unsigned long long> materialIDs{};
		std::for_each(materialObjects.first, materialObjects.second, [&](const auto _id) {
			std::cout << "  first = " << _id.first << ", second = " << _id.second << std::endl;

			// ジオメトリ取得したい
			if (fbxGeometrys.contains(_id.second)) {
				geometryID = _id.second;
			}
			else {
				materialIDs.push_back(_id.second);
			}
		});

		modelMap.insert({ geometryID, materialIDs });
		std::cout << std::endl;
	}
	std::cout << std::endl;

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

		
		std::cout << "Model Vertex Size = " << indexSize << std::endl;
		std::string s{
			"Vertex Size = " + std::to_string(geometry->vertices.size())
			// + ", Index Size = " + std::to_string(geometry->indexes.size())
			+ ", Normal Size = " + std::to_string(geometry->normals.size())
			+ ", UV Size = " + std::to_string(geometry->uvs.size()) };
		std::cout << s << std::endl;

		const size_t ModelVertexSize = indexSize;
		std::vector<model::ModelVertex> modelVertex(ModelVertexSize);
		for (size_t i{ 0 }; i < ModelVertexSize; ++i) {
			const auto indexPointer = geometry->indexes[i];
			modelVertex[i].position = geometry->vertices[indexPointer];
		}

		// material

		// normal
		if (ModelVertexSize != geometry->normals.size()) {
			const size_t normalSize = ModelVertexSize;
			for (size_t i{ 0 }; i < normalSize; ++i) {
				const auto indexPointer = geometry->indexes[i];
				modelVertex[i].normal = geometry->normals[indexPointer];
			}
		}
		else /*if (geometry->indexes.size() == geometry->normals.size())*/ {
			const size_t normalSize = indexSize;
			for (size_t i{ 0 }; i < normalSize; ++i) {

				modelVertex[i].normal = geometry->normals[i];
			}
			// std::cout << 1 << std::endl;
		}

		/*
		// uv
		if (ModelVertexSize != geometry->uvs.size()) {
			for (size_t i{ 0 }; i < geometry->indexes.size(); ++i) {
				const auto index = geometry->indexes[i];
				modelVertex[i].uv = geometry->uvs[index];
			}
		}
		else /* if (geometry->indexes.size() == geometry->uvs.size())  {
			const size_t uvSize = indexSize;
			for (size_t i{ 0 }; i < uvSize; ++i) {
				modelVertex[i].uv = geometry->uvs[i];
			}

			for (size_t i{ 0 }; i < geometry->indexes.size(); ++i) {
				std::cout << "[x = " << modelVertex[i].uv.x
					<< ", y = " << modelVertex[i].uv.y << "]" << std::endl;;
			}
		}
		*/

		const auto& materialIDs = modelMap.find(fbxGeometry.first)->second;
		for (const auto& materialId : materialIDs) {
			if (!fbxMaterials.contains(materialId)) {
				continue;
			}

			if (!materialTextureIds.contains(materialId)) {
				continue;
			}

			const auto& textureId = materialTextureIds.find(materialId)->second;
			const auto& texture = fbxTextures.find(textureId)->second;
			if (texture.surfaceMaterial == "Maya|DiffuseTexture") {
				modelData->materials[idx].textureName = L"C:/Users/tk08g/source/repos/Engine/Asset/FBXModel/" + texture.fileName;
			}
		}
		std::cout << std::endl;

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
