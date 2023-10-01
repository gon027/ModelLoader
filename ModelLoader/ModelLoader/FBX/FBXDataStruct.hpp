#pragma once

#include <vector>

namespace model::fbx {

	struct FBXHeader {
		char magicNumber[32];  // マジックナンバー
		int version{ 0 };      // バージョン
	};

	struct FBXNode {
		// long long endPosition;           // 終端距離
		// long long attributeCount;        // 属性
		// long long allAttributeByteSize;  // 全属性のバイト長
		// char nodeNameSize;               // ノード名のバイト長
		char nodeName[256];              // ノード名
		std::vector<FBXNode> node;       // 子ノード
	};

}