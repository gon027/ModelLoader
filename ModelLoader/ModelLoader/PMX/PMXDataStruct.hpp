#pragma once

#include <vector>
#include <string>
#include <array>
#include <variant>

namespace model::pmx {

	using VString = std::variant <std::string, std::wstring>;

	struct ByteType {
		union {
			struct {
				unsigned char encodeType;
				unsigned char addUV;
				unsigned char vertexIndex;
				unsigned char materialIndex;
				unsigned char textureIndex;
				unsigned char bourneIndex;
				unsigned char morphIndex;
				unsigned char bodyIndex;
			};

			unsigned char m[8];
		};
	};

	// �{�[���C���f�b�N�X
	struct BoneIndex {
		union {
			struct {
				int bone1;
				int bone2;
				int bone3;
				int bone4;
			};

			int m[4];
		};
	};

	// �e�{�[���̃E�F�C�g�l
	struct WeightValue {
		union {
			struct {
				float weight1;
				float weight2;
				float weight3;
				float weight4;
			};

			float m[4];
		};
	};

	// �E�F�C�g
	struct VertexWeight {
		unsigned char weight;
		BoneIndex boneIndex;
		WeightValue weightValue;
		float sdef_c[3];
		float sdef_r0[3];
		float sdef_r1[3];
	};

	namespace DrawFlag {
		constexpr unsigned char BothSides = 0x01;
		constexpr unsigned char GroundShadow = 0x02;
		constexpr unsigned char SelfShadowMap = 0x04;
		constexpr unsigned char Selfshadow = 0x08;
		constexpr unsigned char EdgeDrawing = 0x10;
	};

	enum class SphereMode : unsigned char {
		Invalid = 0,
		Multiply = 1,
		Addition = 2,
		Subtexture = 3,
	};

	/// <summary>
	/// �w�b�_�[��
	/// </summary>
	struct PMXHeader {
		char extension[4];
		float version;
		unsigned char byteSize;
		ByteType byteType;
	};

	/// <summary>
	/// ���f�����
	/// </summary>
	struct ModelInfo {
		VString modelName;
		VString modelNameEnglish;
		VString comment;
		VString commentEnglish;
	};

	/// <summary>
	/// ���_
	/// </summary>
	struct PMXVertex {
		float position[3];
		float normal[3];
		float uv[2];
		std::array<std::array<float, 4>, 4> addUV;
		VertexWeight vertexWeight;
		float edgeScale;
	};


	/// <summary>
	/// �ގ�
	/// </summary>
	struct Material {
		VString materialName;
		VString materialNameEnglish;

		float diffuse[4];
		float specular[3];
		float specularCoefficient;
		float ambient[3];

		unsigned char drawFlag;

		float edgeColor[4];
		float edgeSize;

		int normalTextureIndex;
		int sphereTextureIndex;

		SphereMode mode;

		unsigned char sharedToonFlag;
		unsigned int indexSize;

		VString memo;

		int surface;
	};

	/// <summary>
	/// �{�[��
	/// </summary>
	struct Bourne {
		VString name;
		VString eng;

		float position[3];
		unsigned int parentBoneIndex;
		int henkei;

		int boneFlag;
	};

	/// <summary>
	/// ���[�t
	/// </summary>
	struct Morph {

	};

	/// <summary>
	/// �\���g
	/// </summary>
	struct DisplayFrame {

	};

	/// <summary>
	/// ����
	/// </summary>
	struct Body {

	};

	/// <summary>
	/// Joint
	/// </summary>
	struct joint {

	};


}