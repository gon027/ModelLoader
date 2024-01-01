#include "../FBXLoader.hpp"
#include <zlib/zlib.h>
#include "../../BinaryFile/BinaryFile.hpp"
#include <unordered_map>
#include <filesystem>

// #include <Windows.h>
#include <iostream>

namespace model::fbx {

	namespace {

		unsigned char* inflateData(const char* _inData, unsigned long long _elem, unsigned long long _byte, unsigned long long _typeSize) {
			z_stream zs{};
			zs.zalloc = Z_NULL;
			zs.zfree = Z_NULL;
			zs.opaque = Z_NULL;
			auto ret = inflateInit(&zs);
			if (ret != Z_OK) {
				return nullptr;
			}

			const auto outSize = static_cast<unsigned int>(_elem * _typeSize);
			unsigned char* result = new unsigned char[outSize];
			{
				zs.avail_in = static_cast<unsigned int>(_byte);
				zs.next_in = reinterpret_cast<unsigned char*>(const_cast<char*>(_inData));
				zs.next_out = result;
				zs.avail_out = outSize;
				ret = inflate(&zs, Z_NO_FLUSH);
				if (ret != Z_OK && ret != Z_STREAM_END) {
					return nullptr;
				}
			}
			ret = inflateEnd(&zs);
			if (ret != Z_OK) {
				return nullptr;
			}

			return result;
		}

		template<class type>
		unsigned char* readArrayData(BinaryFile& _bf, unsigned long long _byte, int _elem) {
			std::vector<char> c{};
			c.resize(_byte);
			_bf.read(c.data(), sizeof(char) * _byte);
			auto* outBuf = inflateData(c.data(), _elem, _byte, sizeof(type));
			return outBuf;
		}

		template <class T>
		std::vector<T> getArrayProperty(BinaryFile& _bf) {
			int elems{ _bf.read32() };       // 要素数の取得
			int encode{ _bf.read32() };      // エンコーディング
			int byteLength{ _bf.read32() };  // バイト長

			unsigned char* val{ nullptr };
			if (encode != 0) {
				val = readArrayData<T>(_bf, byteLength, elems);
			}
			else {
				val = new unsigned char[byteLength];
				_bf.read((char*)val, sizeof(char) * byteLength);
			}

			T* cv = reinterpret_cast<T*>(val);
			std::vector<T> result(cv, cv + elems);
			delete[] val;

			return result;
		}

		

		int cnt = 0;
		std::shared_ptr<FBXNode> readNode(BinaryFile& _bf, long long _version) {
			long long dist{ 0 };
			long long attributeNum{ 0 };
			long long attributeTotalBytes{ 0 };

			if (_version >= 7500) {
				dist = _bf.read64();
				attributeNum = _bf.read64();
				attributeTotalBytes = _bf.read64();
			}
			else {
				dist = _bf.read32();
				attributeNum = _bf.read32();
				attributeTotalBytes = _bf.read32();
			}

			char nodeNameTotalBytes{ _bf.read8() };

			// ノード終了
			// <= 7.4 13bytes
			// >= 7.5 25bytes
			if (dist == 0 && attributeNum == 0 && attributeTotalBytes == 0 && nodeNameTotalBytes == 0) {
				return nullptr;
			}


			// ノード名
			char nodeName[64]{};
			_bf.read((char*)&nodeName, sizeof(char) * nodeNameTotalBytes);

			std::shared_ptr<FBXNode> object{ new FBXNode{} };
			object->setName(nodeName);

			// 属性
			for (long long i{ 0 }; i < attributeNum; ++i) {
				char attribute{ _bf.read8() };
				if (attribute == 'S' || attribute == 'R') {
					long long _nodeNameTotalBytes{ _bf.read32() };
					char _nodeName[256]{};
					_bf.read((char*)&_nodeName, sizeof(char) * _nodeNameTotalBytes);
					FBXPropertyPtr property = std::make_shared<FBXStringProperty>(_nodeName);
					object->addPropertys(property);
				}
				else if (attribute == 'C') {
					char value{ _bf.read8() };
					FBXPropertyPtr property = std::make_shared<FBXBoolProperty>(value);
					object->addPropertys(property);
				}
				else if (attribute == 'Y') {
					int value{ _bf.read16() };
					FBXPropertyPtr property = std::make_shared<FBXInt16Property>(value);
					object->addPropertys(property);
				}
				else if (attribute == 'I') {
					int value{ _bf.read32() };
					FBXPropertyPtr property = std::make_shared<FBXInt32Property>(value);
					object->addPropertys(property);
				}
				else if (attribute == 'L') {
					long long value{ _bf.read64() };
					FBXPropertyPtr property = std::make_shared<FBXInt64Property>(value);
					object->addPropertys(property);
				}
				else if (attribute == 'F') {
					float value{ _bf.readFloat() };
					FBXPropertyPtr property = std::make_shared<FBXFloatProperty>(value);
					object->addPropertys(property);
				}
				else if (attribute == 'D') {
					double value{ _bf.readDouble() };
					FBXPropertyPtr property = std::make_shared<FBXDoubleProperty>(value);
					object->addPropertys(property);
				}
				else if (attribute == 'i') { // vec<i32>
					auto value = std::move(getArrayProperty<int>(_bf));
					FBXPropertyPtr property = std::make_shared<FBXInt32ArrayProperty>(value);
					object->addPropertys(property);
				}
				else if (attribute == 'l') { // vec<i64>
					auto value = std::move(getArrayProperty<long long>(_bf));
					FBXPropertyPtr property = std::make_shared<FBXInt64ArrayProperty>(value);
					object->addPropertys(property);
				}
				else if (attribute == 'f') { // vec<f32>
					auto value = std::move(getArrayProperty<float>(_bf));
					FBXPropertyPtr property = std::make_shared<FBXFloatArrayProperty>(value);
					object->addPropertys(property);
				}
				else if (attribute == 'd') { // vec<f64>
					auto value = std::move(getArrayProperty<double>(_bf));
					FBXPropertyPtr property = std::make_shared<FBXDoubleArrayProperty>(value);
					object->addPropertys(property);
				}
				else if (attribute == 'b') { // vec<bool>
					auto value = std::move(getArrayProperty<char>(_bf));
					FBXPropertyPtr property = std::make_shared<FBXBoolArrayProperty>(value);
					object->addPropertys(property);
				}
				else {
					return nullptr;
				}
			}

			// 子ノードがあり、nullptrではなければ
			while (_bf.getPosition() < dist) {
				cnt++;
				std::string a{ 
					"position = " + std::to_string(_bf.getPosition()) 
					+ ", dist = " + std::to_string(dist) 
					+ ", nodeName = " + std::string{ nodeName }
				};
				// std::cout << a << std::endl;
				// OutputDebugString(a.c_str());
				auto child = readNode(_bf, _version);
				cnt--;
				if (child) {
					object->addNode(child);
				}
			}

			return object;
		}

		std::shared_ptr<FBXNode> parseBinaryFBX(BinaryFile& _bf, long long _version) {
			std::shared_ptr<FBXNode> result{ new FBXNode{} };
			result->setName("FBX_ROOT");
			while (!_bf.isEof()) {
				auto child = readNode(_bf, _version);
				if (child) {
					result->addNode(child);
				}
			}

			return result;
		}
	}

	FBXLoader::FBXLoader()
		: version{}
		, globalSetting{ new FBXGlobalSetting{} }
		, fbxGeometrys{}
	{
	}

	FBXLoader::~FBXLoader()
	{
	}

	bool FBXLoader::load(const std::string& _modelDir)
	{
		BinaryFile bf{};
		bf.open(_modelDir);
		if (bf.isFail()) {
			return false;
		}

		char magicNumber[32];
		bf.read(magicNumber, sizeof(char) * 23);
		//std::cout << "マジックナンバー : " << magicNumber << std::endl;

		version = bf.read32();
		//std::cout << "バージョン : " << version << std::endl;

		rootNode = parseBinaryFBX(bf, version);
		if (!rootNode) return false;

		createFbxScene();

		createFBXGeometry();

		createFbxTexture();

		createMaterial();

		createModel();

		createConnections();

		return true;
	}

	void FBXLoader::createFbxScene()
	{
		// GlobalSettingの取得
		{
			auto gSetting = rootNode->findNode("GlobalSettings")->findNode("Properties70");
			if (!gSetting) {
				return;
			}

			// FBXGlobalSetting globalSettings{};
			globalSetting->upAxis = getPropertyValue<int>(gSetting->findPropertyForChildren("UpAxis"));
			globalSetting->upAxisSing = getPropertyValue<int>(gSetting->findPropertyForChildren("UpAxisSign"));
			globalSetting->frontAxis = getPropertyValue<int>(gSetting->findPropertyForChildren("FrontAxis"));
			globalSetting->frontAxisSign = getPropertyValue<int>(gSetting->findPropertyForChildren("FrontAxisSign"));
			globalSetting->coordAxis = getPropertyValue<int>(gSetting->findPropertyForChildren("CoordAxis"));
			globalSetting->coordAxisSign = getPropertyValue<int>(gSetting->findPropertyForChildren("CoordAxisSign"));
		}

	}

	void FBXLoader::createFbxTexture()
	{
		auto objects = rootNode->findNode("Objects");
		// テクスチャ
		auto textures = objects->findNodes("Texture");
		// std::vector<FBXTexture> textureList{};
		for (auto& texture : textures) {
			// auto textureNameProp = texture->findNode("TextureName");
			// if (!textureNameProp) continue;
			// auto textureName = getPropertyValue<std::string>(textureNameProp->getProperty(0));

			auto fileNameProp = texture->findNode("FileName");
			if (!fileNameProp) continue;

			const auto id = getPropertyValue<long long>(texture->getProperty(0));

			auto texture = getPropertyValue<std::string>(fileNameProp->getProperty(0));

			std::filesystem::path textureFilePath{ texture };
			auto texturePath = textureFilePath.filename();

			FBXTexture fbxTexture{};
			fbxTexture.fileName = texturePath;
			// textureList.push_back(fbxTexture);

			fbxTextures.insert({ id, fbxTexture });
		}
	}

	void FBXLoader::createConnections()
	{
		auto connections = rootNode->findNode("Connections");
		static int count{ 0 };
		for (auto idx{ 0 }; idx < connections->getChildrenSize(); ++idx) {
			const auto& child = connections->getChildNode(idx);

			auto connect = getPropertyValue<std::string>(child->getProperty(0));
			if (connect == "OO") {
				auto o1 = getPropertyValue<long long>(child->getProperty(1));
				auto o2 = getPropertyValue<long long>(child->getProperty(2));
				oos.insert({ o1, o2 });
			}
			else if (connect == "OP") {
				auto o1 = getPropertyValue<long long>(child->getProperty(1));
				auto o2 = getPropertyValue<long long>(child->getProperty(2));
				auto diffuseColor = getPropertyValue<std::string>(child->getProperty(3));
				ops.insert({ o1, o2 });
			}
			else if (connect == "PO"){
				++count;
			}
			else {
				++count;
			}
		}
	}

	void FBXLoader::createMaterial()
	{
		auto objects = rootNode->findNode("Objects");
		auto materials = objects->findNodes("Material");
		if (materials.size() == 0) return;

		for (auto& material : materials) {
			const auto id = getPropertyValue<long long>(material->getProperty(0));

			fbxMaterial.insert({ id, {} });
		}

	}

	void FBXLoader::createModel()
	{
		auto objects = rootNode->findNode("Objects");
		auto models = objects->findNodes("Model");
		if (models.size() == 0) return;

		std::vector<FBXNode::FBXNodePtr> meshModels{};
		for (auto& model : models) {
			auto propStr = getPropertyValue<std::string>(model->getProperty(2));
			if (propStr != "Mesh") continue;
			meshModels.push_back(model);
		}

		for (auto& meshModel : meshModels) {
			const auto id = getPropertyValue<long long>(meshModel->getProperty(0));

			auto props = meshModel->findNode("Properties70");
			
			FBXModel fbxModel{};
			for (size_t idx{ 0 }; idx < props->getChildrenSize(); ++idx) {
				const auto& elem = props->getChildNode(idx);

				const auto& ps = getPropertyValue<std::string>(elem->getProperty(0));
				if (ps == "Lcl Translation") {
					fbxModel.lclTranslation[0] = (float)(getPropertyValue<double>(elem->getProperty(4)));
					fbxModel.lclTranslation[1] = (float)(getPropertyValue<double>(elem->getProperty(5)));
					fbxModel.lclTranslation[2] = (float)(getPropertyValue<double>(elem->getProperty(6)));
				}
				else if (ps == "Lcl Rotation") {
					fbxModel.lclRotation[0] = (float)(getPropertyValue<double>(elem->getProperty(4)));
					fbxModel.lclRotation[1] = (float)(getPropertyValue<double>(elem->getProperty(5)));
					fbxModel.lclRotation[2] = (float)(getPropertyValue<double>(elem->getProperty(6)));
				}
				else if (ps == "Lcl Scaling") {
					fbxModel.lclScaling[0] = (float)(getPropertyValue<double>(elem->getProperty(4)));
					fbxModel.lclScaling[1] = (float)(getPropertyValue<double>(elem->getProperty(5)));
					fbxModel.lclScaling[2] = (float)(getPropertyValue<double>(elem->getProperty(6)));
				}
			}
			fbxModels.insert({ id, fbxModel });
		}
	}

	void FBXLoader::createFBXGeometry()
	{
		auto objects = rootNode->findNode("Objects");
		auto geometrys = objects->findNodes("Geometry");
		if (geometrys.size() == 0) {
			return;
		}

		const auto coordAxis = globalSetting->coordAxis;
		const auto upAxis = globalSetting->upAxis;
		const auto frontAxis = globalSetting->frontAxis;

		const auto coordAxisSign = globalSetting->coordAxisSign;
		const auto upAxisSign = globalSetting->upAxisSing;
		const auto frontAxisSign = globalSetting->frontAxisSign;

		std::vector<model::fbx::FBXNode::FBXNodePtr> meshes{};
		for (auto& geometry : geometrys) {
			auto propertyNameProp = geometry->getProperty(geometry->getPropertysSize() - 1);
			auto propertyName = getPropertyValue<std::string>(propertyNameProp);
			if (propertyName == "Mesh") {
				meshes.emplace_back(geometry);
			}
		}

		// Vertex
		std::vector<std::vector<Vertex3>> verteies{};
		{
			for (auto& mesh : meshes) {
				auto vertexProp = mesh->findNode("Vertices")->getProperty(0);
				auto vertex = getPropertyValue<std::vector<double>>(vertexProp);

				// Todo: FBXの配列を変換する
				std::vector<Vertex3> retVertex(vertex.size() / 3);
				for (size_t idx{ 0 }; idx < vertex.size(); idx += 3) {
					const size_t reslutVertexIdx{ idx / 3 };
					retVertex[reslutVertexIdx].x = static_cast<float>(coordAxisSign * vertex[idx + coordAxis]);
					retVertex[reslutVertexIdx].y = static_cast<float>(upAxisSign * vertex[idx + upAxis]);
					retVertex[reslutVertexIdx].z = static_cast<float>(frontAxisSign * vertex[idx + frontAxis]);
				}

				verteies.push_back(retVertex);
			}
		}

		// Index
		std::vector<std::vector<uint16_t>> indeies{};
		{
			int ccc{ 0 };
			for (auto& mesh : meshes) {
				bool isPloygonVertexIndex = true;

				auto indexProp = mesh->findNode("PolygonVertexIndex");
				if (!indexProp) {
					isPloygonVertexIndex = false;
					indexProp = mesh->findNode("Indexes");
					if (!indexProp) {
						return;
					}
				}

				std::vector<uint16_t> tmpIndex{};
				size_t tmpIndexSize{ 0 };
				size_t currentTmpIndexPosition{ 0 };
				auto index = getPropertyValue<std::vector<int>>(indexProp->getProperty(0));
				if (isPloygonVertexIndex) {

					size_t idx{ 0 };
					while (idx < index.size()) {
						size_t offset{ idx };
						size_t countToNegativeIndex{ 0 };
						while (!(index[offset] < 0)) {
							++countToNegativeIndex;
							++offset;
						}
						++countToNegativeIndex;

						if (countToNegativeIndex == 3) {
							tmpIndexSize += 3;
							tmpIndex.resize(tmpIndexSize);

							for (size_t i{ 0 }; i < 3; ++i) {
								const size_t currentIdx = idx + i;
								tmpIndex[currentTmpIndexPosition] = index[currentIdx];
								if (index[currentIdx] < 0) {
									uint16_t pulsIndex = ~index[currentIdx];
									tmpIndex[currentTmpIndexPosition] = pulsIndex;
								}
								++currentTmpIndexPosition;
							}
							idx += 3;
						}
						else {
							tmpIndexSize += 3 * (countToNegativeIndex - 2);
							tmpIndex.resize(tmpIndexSize);

							for (size_t i{ 0 }; i < 3; ++i) {
								const size_t currentIdx = idx + i;
								tmpIndex[currentTmpIndexPosition] = index[currentIdx];
								++currentTmpIndexPosition;
							}
							idx += 3;

							// 5 .. 2
							const size_t loopCount = (countToNegativeIndex - 2) - 1;
							for (size_t i{ 0 }; i < loopCount; ++i) {
								const size_t currentIdx = idx + i;

								tmpIndex[currentTmpIndexPosition + 0] = tmpIndex[currentTmpIndexPosition - 3];
								tmpIndex[currentTmpIndexPosition + 1] = tmpIndex[currentTmpIndexPosition - 1];

								if (index[currentIdx] < 0) {
									uint16_t pulsIndex = ~index[currentIdx];
									tmpIndex[currentTmpIndexPosition + 2] = pulsIndex;
								}
								else {
									tmpIndex[currentTmpIndexPosition + 2] = index[currentIdx];
								}
								currentTmpIndexPosition += 3;
							}

							idx += loopCount;
						}
					}
				}
				else {
					tmpIndex.resize(index.size());
					std::copy(index.begin(), index.end(), tmpIndex.begin());
				}

				indeies.push_back(tmpIndex);
			}
		}

		// normals
		std::vector<std::vector<Vertex3>> normals{};
		{
			for (auto& mesh : meshes) {
				auto layerElementNormalProp = mesh->findNode("LayerElementNormal");

				auto mappingInfomationTypeProp = layerElementNormalProp->findNode("MappingInformationType");
				if (!mappingInfomationTypeProp) continue;
				auto mappingInfomationType = getPropertyValue<std::string>(mappingInfomationTypeProp->getProperty(0));

				auto referenceInformationTypeProp = layerElementNormalProp->findNode("ReferenceInformationType");
				if (!referenceInformationTypeProp) continue;
				auto referenceInformationType = getPropertyValue<std::string>(referenceInformationTypeProp->getProperty(0));

				auto normalsProp = layerElementNormalProp->findNode("Normals");
				if (!normalsProp) continue;
				std::vector<double> normalsVec = std::move(getPropertyValue<std::vector<double>>(normalsProp->getProperty(0)));
				const auto normalsSize = normalsVec.size();

				if (mappingInfomationType == "ByVertice") {  // ByControlPoint
					if (referenceInformationType == "Direct") {
						// 直接
						std::vector<Vertex3> retNormals(normalsSize / 3);
						for (size_t idx{ 0 }; idx < normalsSize; idx += 3) {
							const size_t resultNormalIdx{ idx / 3 };
							retNormals[resultNormalIdx].x = static_cast<float>(normalsVec[idx + coordAxis]);
							retNormals[resultNormalIdx].y = static_cast<float>(normalsVec[idx + upAxis]);
							retNormals[resultNormalIdx].z = static_cast<float>(normalsVec[idx + frontAxis]);
						}
						normals.push_back(retNormals);
					}
					// else if (referenceInformationType == "IndexToDirect") {
					// 考慮しなくてよい
					// }
				}
				else if (mappingInfomationType == "ByPolygonVertex") {
					if (referenceInformationType == "Direct") {
						// 直接
						std::vector<Vertex3> retNormals(normalsSize / 3);
						for (size_t idx{ 0 }; idx < normalsSize; idx += 3) {
							const size_t resultNormalIdx{ idx / 3 };
							retNormals[resultNormalIdx].x = static_cast<float>(normalsVec[idx + coordAxis]);
							retNormals[resultNormalIdx].y = static_cast<float>(normalsVec[idx + upAxis]);
							retNormals[resultNormalIdx].z = static_cast<float>(normalsVec[idx + frontAxis]);
						}
						normals.push_back(retNormals);
					}
					else if (referenceInformationType == "IndexToDirect") {
						// Todo: 今のところ見ないので、見たら実装
					}
				}
			}
		}

		// uvs
		std::unordered_map<int, std::vector<std::vector<Vertex2>>> uvs{};
		{
			int uvId{ 0 };
			for (auto& mesh : meshes) {
				auto layerElementUVNodes = mesh->findNodes("LayerElementUV");
				if (layerElementUVNodes.size() == 0) continue;

				std::vector<std::vector<Vertex2>> uvList{};
				for (auto& layerElementUvNode : layerElementUVNodes) {
					auto mappingInfomationTypeProp = layerElementUvNode->findNode("MappingInformationType");
					if (!mappingInfomationTypeProp) continue;
					auto mappingInfomationType = getPropertyValue<std::string>(mappingInfomationTypeProp->getProperty(0));

					auto referenceInformationTypeProp = layerElementUvNode->findNode("ReferenceInformationType");
					if (!referenceInformationTypeProp) continue;
					auto referenceInformationType = getPropertyValue<std::string>(referenceInformationTypeProp->getProperty(0));

					auto uvProp = layerElementUvNode->findNode("UV");
					if (!uvProp) continue;
					std::vector<double> uvVec = std::move(getPropertyValue<std::vector<double>>(uvProp->getProperty(0)));
					const auto uvSize = uvVec.size();

					if (mappingInfomationType == "ByVertice") {  // ByControlPoint
						if (referenceInformationType == "Direct") {

						}
						else if (referenceInformationType == "IndexToDirect") {
						}
					}
					else if (mappingInfomationType == "ByPolygonVertex") {
						if (referenceInformationType == "Direct") {
							std::vector<Vertex2> retUvs(uvSize / 2);
							for (size_t idx{ 0 }; idx < uvSize; idx += 2) {
								const auto uvIdx = idx / 2;
								retUvs[uvIdx].x = static_cast<float>(uvVec[uvIdx + 0]);
								retUvs[uvIdx].y = static_cast<float>(uvVec[uvIdx + 1]);
							}

							uvList.push_back(retUvs);
						}
						else if (referenceInformationType == "IndexToDirect") {
							auto uvIndexProp = layerElementUvNode->findNode("UVIndex");
							std::vector<int> uvIndexVec = std::move(getPropertyValue<std::vector<int>>(uvIndexProp->getProperty(0)));

							std::vector<Vertex2> tmpUvs(uvSize / 2);
							for (size_t idx{ 0 }; idx < uvSize; idx += 2) {
								const auto uvIdx = idx / 2;
								tmpUvs[uvIdx].x = static_cast<float>(uvVec[uvIdx + 0]);
								tmpUvs[uvIdx].y = static_cast<float>(uvVec[uvIdx + 1]);
							}

							const auto maxUvSize = std::max(tmpUvs.size(), uvIndexVec.size());
							std::vector<Vertex2> retUvs(maxUvSize);
							for (size_t idx{ 0 }; idx < maxUvSize; ++idx) {
								retUvs[idx] = tmpUvs[uvIndexVec[idx]];
							}
							uvList.push_back(retUvs);
						}
					}
				}

				uvs.insert({ uvId, uvList });
				++uvId;
			}
		}

		size_t size = meshes.size();
		// for (size_t idx{ 0 }; idx < size; ++idx) {
		size_t idx{ 0 }; 
		for (auto& mesh : meshes) {
			const auto id = getPropertyValue<long long>(mesh->getProperty(0));

			std::shared_ptr<FBXGeometry> geometry{ new FBXGeometry{} };
			geometry->vertices = std::move(verteies[idx]);
			geometry->indexes = std::move(indeies[idx]);
			geometry->normals = std::move(normals[idx]);
			geometry->uvs = std::move(uvs[idx][0]);
			fbxGeometrys.insert({ id, geometry });

			++idx;
		}
	}
}