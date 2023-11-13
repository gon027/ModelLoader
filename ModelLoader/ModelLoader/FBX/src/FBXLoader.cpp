#include "../FBXLoader.hpp"
#include <zlib/zlib.h>
#include "../../BinaryFile/BinaryFile.hpp"

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
			int elems{ _bf.read32() };       // �v�f���̎擾
			int encode{ _bf.read32() };      // �G���R�[�f�B���O
			int byteLength{ _bf.read32() };  // �o�C�g��

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

			// �m�[�h�I��
			// <= 7.4 13bytes
			// >= 7.5 25bytes
			if (dist == 0 && attributeNum == 0 && attributeTotalBytes == 0 && nodeNameTotalBytes == 0) {
				return nullptr;
			}

			// �m�[�h��
			char nodeName[64]{};
			_bf.read((char*)&nodeName, sizeof(char) * nodeNameTotalBytes);

			std::shared_ptr<FBXNode> object{ new FBXNode{} };
			object->setName(nodeName);

			// ����
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

			// �q�m�[�h������Anullptr�ł͂Ȃ����
			while (_bf.getPosition() < dist) {
				auto child = readNode(_bf, _version);
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
        //std::cout << "�}�W�b�N�i���o�[ : " << magicNumber << std::endl;

        version = bf.read32();
        //std::cout << "�o�[�W���� : " << version << std::endl;

		rootNode = parseBinaryFBX(bf, version);
		if (!rootNode) return false;

		createFbxScene();

        return true;
    }

	void FBXLoader::createFbxScene()
	{
		// GlobalSetting�̎擾
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

		fbxGeometrys = std::move(createFBXGeometry());
	}

	std::vector<std::shared_ptr<FBXGeometry>> FBXLoader::createFBXGeometry()
	{
		std::vector<std::shared_ptr<FBXGeometry>> result{};

		auto objects = rootNode->findNode("Objects");
		auto geometrys = objects->findNodes("Geometry");
		if (geometrys.size() == 0) {
			return {};
		}

		auto coordAxis = globalSetting->coordAxis;
		auto upAxis = globalSetting->upAxis;
		auto frontAxis = globalSetting->frontAxis;

		// Vertex
		std::vector<std::vector<float>> verteies{};
		{
			for (auto& geometry : geometrys) {
				// Todo: Property��Shape�̎��̓X�L�b�v����
				auto propertyNameProp = geometry->getProperty(geometry->getPropertysSize() - 1);
				auto propertyName = getPropertyValue<std::string>(propertyNameProp);
				if (propertyName != "Mesh") continue;
				
				auto vertexProp = geometry->findNode("Vertices")->getProperty(0);
				auto vertex = getPropertyValue<std::vector<double>>(vertexProp);
				
				// Todo: FBX�̔z���ϊ�����
				std::vector<float> resVertex(vertex.size());
				for (size_t idx{ 0 }; idx < vertex.size(); idx += 3) {
					resVertex[idx + 0] = static_cast<float>(vertex[idx + coordAxis]);
					resVertex[idx + 1] = static_cast<float>(vertex[idx + upAxis]);
					resVertex[idx + 2] = static_cast<float>(vertex[idx + frontAxis]);
				}

				verteies.push_back(resVertex);
			}
		}

		// Index
		std::vector<std::vector<uint16_t>> indeies{};
		{
			for (auto& geometry : geometrys) {
				auto propertyNameProp = geometry->getProperty(geometry->getPropertysSize() - 1);
				auto propertyName = getPropertyValue<std::string>(propertyNameProp);
				if (propertyName != "Mesh") continue;

				bool isPloygonVertexIndex = true;

				auto indexProp = geometry->findNode("PolygonVertexIndex");
				if (!indexProp) {
					isPloygonVertexIndex = false;
					indexProp = geometry->findNode("Indexes");
					if (!indexProp) {
						return {};
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
							tmpIndexSize += 6;
							tmpIndex.resize(tmpIndexSize);

							for (size_t i{ 0 }; i < 4; ++i) {
								const size_t currentIdx = idx + i;
								tmpIndex[currentTmpIndexPosition] = index[currentIdx];

								if (index[currentIdx] < 0) {
									uint16_t pulsIndex = ~index[currentIdx];

									tmpIndex[currentTmpIndexPosition + 0] = tmpIndex[currentTmpIndexPosition - 3];
									tmpIndex[currentTmpIndexPosition + 1] = tmpIndex[currentTmpIndexPosition - 1];
									tmpIndex[currentTmpIndexPosition + 2] = pulsIndex;

									currentTmpIndexPosition += 3;
									idx += 4;
									break;
								}
								++currentTmpIndexPosition;
							}
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
		std::vector<std::vector<float>> normals{};
		{
			for (auto& geometry : geometrys) {
				auto propertyNameProp = geometry->getProperty(geometry->getPropertysSize() - 1);
				auto propertyName = getPropertyValue<std::string>(propertyNameProp);
				if (propertyName != "Mesh") continue;

				auto layerElementNormalProp = geometry->findNode("LayerElementNormal");

				std::vector<double> normalsVec{};
				if (layerElementNormalProp)
				{
					auto mappingInfomationTypeProp = layerElementNormalProp->findNode("MappingInformationType");
					if (!mappingInfomationTypeProp) return {};
					auto mappingInfomationType = getPropertyValue<std::string>(mappingInfomationTypeProp->getProperty(0));

					auto referenceInformationTypeProp = layerElementNormalProp->findNode("ReferenceInformationType");
					if (!referenceInformationTypeProp) return {};
					auto referenceInformationType = getPropertyValue<std::string>(referenceInformationTypeProp->getProperty(0));

					auto normalsProp = layerElementNormalProp->findNode("Normals");
					if (!normalsProp) return {};
					normalsVec = std::move(getPropertyValue<std::vector<double>>(normalsProp->getProperty(0)));
				}
				else {
					auto normalsProp = geometry->findNode("Normals");
					if (!normalsProp) return {};
					normalsVec = std::move(getPropertyValue<std::vector<double>>(normalsProp->getProperty(0)));
				}

				const auto normalsSize = normalsVec.size();
				std::vector<float> retNormals(normalsSize);
				for (size_t idx{ 0 }; idx < normalsSize; idx += 3) {
					retNormals[idx + coordAxis] = static_cast<float>(normalsVec[idx + coordAxis]);
					retNormals[idx + upAxis] = static_cast<float>(normalsVec[idx + upAxis]);
					retNormals[idx + frontAxis] = static_cast<float>(normalsVec[idx + frontAxis]);
				}

				normals.push_back(retNormals);
			}
		}

		size_t size = verteies.size();
		for (size_t idx{ 0 }; idx < size; ++idx) {
			std::shared_ptr<FBXGeometry> geometry{ new FBXGeometry{} };
			geometry->vertices = std::move(verteies[idx]);
			geometry->indexes = std::move(indeies[idx]);
			geometry->normals = std::move(normals[idx]);
			result.push_back(geometry);
		}

		return result;
	}

}