#include "../PMXLoader.hpp"

#include <fstream>
#include <string>

namespace model::pmx {

	namespace {

		VString readString(std::ifstream& _ifs, unsigned char _encodeType) {

			unsigned int textSize;
			_ifs.read((char*)&textSize, sizeof(textSize));

			VString resultStr;

			// wchar
			if (_encodeType == 0) {
				wchar_t* wc = new wchar_t[textSize];
				_ifs.read((char*)wc, textSize);

				std::wstring wstr{ wc, wc + textSize / 2 };
				delete[] wc;

				resultStr = wstr;
			}

			// cahr
			if (_encodeType == 1) {
				char* ch = new char[textSize];
				_ifs.read(ch, textSize);

				std::string str{ ch, ch + textSize };
				delete[] ch;

				resultStr = str;
			}


			return resultStr;
		}

		template<class T>
		void readBinaryData(std::ifstream& _ifs, T& _data) {
			_ifs.read(reinterpret_cast<char*>(std::addressof(_data)), sizeof(_data));
		}

		template<class T>
		void readBinaryData(std::ifstream& _ifs, T& _data, std::streamsize _streamsize) {
			_ifs.read(reinterpret_cast<char*>(std::addressof(_data)), _streamsize);
		}

	}

	PMXLoader::PMXLoader()
		: pmxFile()
	{
	}

	bool PMXLoader::load(const std::string& _modelDir, const std::string& _modelFile, const std::string _texDir)
	{

		std::ifstream ifs{ _modelDir + _modelFile , std::ios::in | std::ios::binary };
		if (!ifs) {
			return false;
		}

		// auto& header = pmxFile.header;
		PMXHeader header{};
		{
			ifs.read((char*)&header.extension, sizeof(header.extension));
			ifs.read((char*)&header.version, sizeof(header.version));
			ifs.read((char*)&header.byteSize, sizeof(header.byteSize));
			ifs.read((char*)&header.byteType.m, sizeof(header.byteType.m));
		}

		{
			auto& modelInfo = pmxFile.modelInfo;
			modelInfo.modelName = readString(ifs, header.byteType.encodeType);
			modelInfo.modelNameEnglish = readString(ifs, header.byteType.encodeType);
			modelInfo.comment = readString(ifs, header.byteType.encodeType);
			modelInfo.commentEnglish = readString(ifs, header.byteType.encodeType);
		}

		{
			int vertexSize;
			readBinaryData(ifs, vertexSize);

			pmxFile.vertexes.resize(vertexSize);
			auto& vertexes = pmxFile.vertexes;
			for (size_t i{ 0 }; i < vertexSize; ++i) {
				readBinaryData(ifs, vertexes[i].position);
				readBinaryData(ifs, vertexes[i].normal);
				readBinaryData(ifs, vertexes[i].uv);

				readBinaryData(ifs, vertexes[i].addUV, 16LL * header.byteType.addUV);

				unsigned char weight{};
				readBinaryData(ifs, weight);
				switch (weight)
				{
				case 0: // BDEF1
				{
					readBinaryData(ifs, vertexes[i].vertexWeight.boneIndex.m, header.byteType.bourneIndex);
					break;
				}
				case 1: // BDEF2
				{
					readBinaryData(ifs, vertexes[i].vertexWeight.boneIndex.m, header.byteType.bourneIndex * 2LL);
					readBinaryData(ifs, vertexes[i].vertexWeight.weightValue.m, sizeof(float));
					break;
				}
				case 2: // BDEF4
				{
					readBinaryData(ifs, vertexes[i].vertexWeight.boneIndex.m, header.byteType.bourneIndex * 4LL);
					readBinaryData(ifs, vertexes[i].vertexWeight.weightValue.m, sizeof(float) * 4LL);
					break;
				}
				case 3: // SDEF
				{
					readBinaryData(ifs, vertexes[i].vertexWeight.boneIndex.m, header.byteType.bourneIndex * 2LL);
					readBinaryData(ifs, vertexes[i].vertexWeight.weightValue.m, sizeof(float));

					readBinaryData(ifs, vertexes[i].vertexWeight.sdef_c);
					readBinaryData(ifs, vertexes[i].vertexWeight.sdef_r0);
					readBinaryData(ifs, vertexes[i].vertexWeight.sdef_r1);

					break;
				}
				default:
					break;
				}


				readBinaryData(ifs, vertexes[i].edgeScale);
			}
		}

		{
			int indexSize;
			readBinaryData(ifs, indexSize);

			pmxFile.indexes.resize(indexSize);
			auto& indexes = pmxFile.indexes;
			for (size_t i{ 0 }; i < indexSize; ++i) {
				readBinaryData(ifs, indexes[i], header.byteType.vertexIndex);
			}
		}

		{
			int textureSize;
			readBinaryData(ifs, textureSize);

			pmxFile.textures.resize(textureSize);
			auto& textures = pmxFile.textures;
			for (size_t i{ 0 }; i < textureSize; ++i) {
				textures[i] = readString(ifs, header.byteType.encodeType);
			}
		}

		{
			int materialSize;
			readBinaryData(ifs, materialSize);

			pmxFile.materials.resize(materialSize);
			auto& materials = pmxFile.materials;
			for (size_t i{ 0 }; i < materialSize; ++i) {
				materials[i].materialName = readString(ifs, header.byteType.encodeType);
				materials[i].materialNameEnglish = readString(ifs, header.byteType.encodeType);

				readBinaryData(ifs, materials[i].diffuse);
				readBinaryData(ifs, materials[i].specular);
				readBinaryData(ifs, materials[i].specularCoefficient);
				readBinaryData(ifs, materials[i].ambient);

				readBinaryData(ifs, materials[i].drawFlag);

				readBinaryData(ifs, materials[i].edgeColor);
				readBinaryData(ifs, materials[i].edgeSize);

				readBinaryData(ifs, materials[i].normalTextureIndex, header.byteType.materialIndex);
				readBinaryData(ifs, materials[i].sphereTextureIndex, header.byteType.materialIndex);

				unsigned char sphereMode;
				readBinaryData(ifs, sphereMode);
				switch (sphereMode)
				{
				case 0:
				{
					materials[i].mode = SphereMode::Invalid;
					break;
				}
				case 1:
				{
					materials[i].mode = SphereMode::Multiply;
					break;
				}
				case 2:
				{
					materials[i].mode = SphereMode::Addition;
					break;
				}
				case 3:
				{
					materials[i].mode = SphereMode::Subtexture;
					break;
				}
				default:
					break;
				}

				readBinaryData(ifs, materials[i].sharedToonFlag);
				if (materials[i].sharedToonFlag == 0) {
					readBinaryData(ifs, materials[i].indexSize, header.byteType.materialIndex);
				}
				else if (materials[i].sharedToonFlag == 1) {
					readBinaryData(ifs, materials[i].indexSize, sizeof(unsigned char));
				}

				materials[i].memo = readString(ifs, header.byteType.encodeType);

				readBinaryData(ifs, materials[i].surface);

			}
		}

		return true;
	}

}