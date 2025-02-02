#include "../../include/PMX/PMXLoader.hpp"
#include <fstream>
#include <string>
#include <variant>
#include "../../include/Utility/StringUtility.hpp"

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

		constexpr short AccessPoint  = 0x0001;
		constexpr short Rotateble    = 0x0002;
		constexpr short Translatable = 0x0004;
		constexpr short Display      = 0x0008;
		constexpr short Controllable = 0x0010;

		constexpr short IK = 0x0020;

		constexpr short LocalGrant     = 0x0080;
		constexpr short RotateGrant    = 0x0100;
		constexpr short TranslateGrant = 0x0200;

		constexpr short FixedAxis = 0x0400;
		constexpr short LocalAxis = 0x0800;

		constexpr short Physical = 0x1000;
		constexpr short External = 0x2000;

	}

	PMXLoader::PMXLoader()
		: pmxFile()
	{
	}

	bool PMXLoader::load(const std::string& _modelDir, const std::string& _modelFile)
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
			// std::cout << vertexSize << std::endl;

			pmxFile.vertexes.resize(vertexSize);
			auto& vertexes = pmxFile.vertexes;
			for (size_t i{ 0 }; i < vertexSize; ++i) {
				readBinaryData(ifs, vertexes[i].position);
				readBinaryData(ifs, vertexes[i].normal);
				readBinaryData(ifs, vertexes[i].uv);

				readBinaryData(ifs, vertexes[i].addUV, sizeof(float[4]) * header.byteType.addUV);

				unsigned char weight{};
				readBinaryData(ifs, weight);
				// std::cout << (int)weight << std::endl;
				switch (weight)
				{
				case 0: // BDEF1
				{
					readBinaryData(ifs, vertexes[i].vertexWeight.boneIndex.m, header.byteType.bourneIndex);
					break;
				}
				case 1: // BDEF2
				{
					readBinaryData(ifs, vertexes[i].vertexWeight.boneIndex.m[0], header.byteType.bourneIndex);
					readBinaryData(ifs, vertexes[i].vertexWeight.boneIndex.m[1], header.byteType.bourneIndex);
					readBinaryData(ifs, vertexes[i].vertexWeight.weightValue.m, sizeof(float));
					break;
				}
				case 2: // BDEF4
				{
					readBinaryData(ifs, vertexes[i].vertexWeight.boneIndex.m[0], header.byteType.bourneIndex);
					readBinaryData(ifs, vertexes[i].vertexWeight.boneIndex.m[1], header.byteType.bourneIndex);
					readBinaryData(ifs, vertexes[i].vertexWeight.boneIndex.m[2], header.byteType.bourneIndex);
					readBinaryData(ifs, vertexes[i].vertexWeight.boneIndex.m[3], header.byteType.bourneIndex);
					readBinaryData(ifs, vertexes[i].vertexWeight.weightValue.m, sizeof(float) * 4LL);
					break;
				}
				case 3: // SDEF
				{
					readBinaryData(ifs, vertexes[i].vertexWeight.boneIndex.m[0], header.byteType.bourneIndex);
					readBinaryData(ifs, vertexes[i].vertexWeight.boneIndex.m[1], header.byteType.bourneIndex);
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

			auto modelDirectory = util::convertToWString(_modelDir);
			for (size_t i{ 0 }; i < textureSize; ++i) {
				auto texturePath = readString(ifs, header.byteType.encodeType);
				
				if (std::holds_alternative<std::string>(texturePath)) {
					textures[i] = { _modelDir + std::get<std::string>(texturePath) };
				}

				if (std::holds_alternative<std::wstring>(texturePath)) {
					textures[i] = { modelDirectory + std::get<std::wstring>(texturePath) };
				}
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

		/*
		// �{�[��
		{
			int bourneSize;
			readBinaryData(ifs, bourneSize);

			pmxFile.bournes.resize(bourneSize);
			auto& bournes = pmxFile.bournes;
			for (size_t i{ 0 }; i < bourneSize; ++i) {
				bournes[i].bourneName = readString(ifs, header.byteType.encodeType);
				bournes[i].bourneNameEnglish = readString(ifs, header.byteType.encodeType);

				readBinaryData(ifs, bournes[i].position);
				readBinaryData(ifs, bournes[i].parentBoneIndex, header.byteType.morphIndex);
				readBinaryData(ifs, bournes[i].henkei);

				readBinaryData(ifs, bournes[i].bourneFlag);

				auto bourneFlag = bournes[i].bourneFlag;
				if (auto a = bourneFlag & AccessPoint) {
					readBinaryData(ifs, bournes[i].accessPointIndex, header.byteType.morphIndex);
				}
				else {
					readBinaryData(ifs, bournes[i].positionOffset);
				}

				if ((bourneFlag & Rotateble) || (bourneFlag & Translatable)) {
					readBinaryData(ifs, bournes[i].grantBourneIndex, header.byteType.morphIndex);
					readBinaryData(ifs, bournes[i].grantRate);
				}

				
				// if (bourneFlag & Display) {
				// 	readBinaryData(ifs, bournes[i].grantBourneIndex);
				// }
				// 
				// if (bourneFlag & Controllable) {
				// 	readBinaryData(ifs, bournes[i].grantBourneIndex);
				// }
				

				if (bourneFlag & FixedAxis) {
					readBinaryData(ifs, bournes[i].axis);
				}

				if (bourneFlag & LocalAxis) {
					readBinaryData(ifs, bournes[i].xAxis);
					readBinaryData(ifs, bournes[i].zAxis);
				}

				if (bourneFlag & External) {
					readBinaryData(ifs, bournes[i].key);
				}

				if (bourneFlag & IK) {
					readBinaryData(ifs, bournes[i].ikTargetBourneIndex, header.byteType.morphIndex);
					readBinaryData(ifs, bournes[i].ikLoopCount);
					readBinaryData(ifs, bournes[i].ikLoopRadian);

					readBinaryData(ifs, bournes[i].ikLinkCount);

					pmxFile.bournes[i].ikLinks.resize(bournes[i].ikLinkCount);
					auto& ikLinks = pmxFile.bournes[i].ikLinks;
					for (size_t _ik{ 0 }; _ik < bournes[i].ikLinkCount; ++i) {
						readBinaryData(ifs, ikLinks[_ik].ikLinkBourneIndex);

						readBinaryData(ifs, ikLinks[_ik].angleLimit);
						if (!ikLinks[_ik].angleLimit) {
							readBinaryData(ifs, ikLinks[_ik].upperAngle); 
							readBinaryData(ifs, ikLinks[_ik].lowerAngle);
						}
					}
				}
			}
		}
		*/

		return true;
	}

}