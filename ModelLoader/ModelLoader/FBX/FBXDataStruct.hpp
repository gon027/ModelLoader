#pragma once

#include <vector>

namespace model::fbx {

	struct FBXHeader {
		char magicNumber[32];  // �}�W�b�N�i���o�[
		int version{ 0 };      // �o�[�W����
	};

	struct FBXNode {
		// long long endPosition;           // �I�[����
		// long long attributeCount;        // ����
		// long long allAttributeByteSize;  // �S�����̃o�C�g��
		// char nodeNameSize;               // �m�[�h���̃o�C�g��
		char nodeName[256];              // �m�[�h��
		std::vector<FBXNode> node;       // �q�m�[�h
	};

}