#include "../FBXLoader.hpp"
#include <zlib/zlib.h>
#include "../../BinaryFile/BinaryFile.hpp"

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

			const auto size = sizeof(T) * elems;
			std::vector<T> val2{};
			val2.resize(size);
			memcpy_s(val2.data(), size, val, size);

			delete[] val;
			return val2;
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

			FbxGlobalSetting globalSettings{};
			globalSettings.upAxis = getPropertyValue<int>(gSetting->findPropertyForChildren("UpAxis"));
			globalSettings.upAxisSing = getPropertyValue<int>(gSetting->findPropertyForChildren("UpAxisSign"));
			globalSettings.frontAxis = getPropertyValue<int>(gSetting->findPropertyForChildren("FrontAxis"));
			globalSettings.frontAxisSign = getPropertyValue<int>(gSetting->findPropertyForChildren("FrontAxisSign"));
			globalSettings.coordAxis = getPropertyValue<int>(gSetting->findPropertyForChildren("CoordAxis"));
			globalSettings.coordAxisSign = getPropertyValue<int>(gSetting->findPropertyForChildren("CoordAxisSign"));
		}


		{

		}
	}

}