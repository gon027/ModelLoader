#pragma once

#include <string>
#include <vector>
#include <memory>
#include <variant>

#include "FBXProperty.hpp"

namespace model::fbx {

	class FBXNode {
	public:
		using FBXPerentNode = std::weak_ptr<FBXNode>;
		using FBXNodePtr = std::shared_ptr<FBXNode>;

	public:
		FBXNode();
		FBXNode(const std::string& _name);
		~FBXNode() = default;

		void setName(const std::string& _name);

		void addNode(FBXNodePtr _node);

		// void setPerentNode(FBXPerentNode _perentNode);

		void addPropertys(FBXPropertyPtr _property);

		FBXNodePtr findNode(const std::string& _name);

		std::vector<FBXNodePtr> findNodes(const std::string& _name);

		FBXPropertyPtr findProperty(const std::string& _name);

		FBXPropertyPtr findPropertyForChildren(const std::string& _name);

		inline std::string getNodeName() const {
			return name;
		}

		inline size_t getChildrenSize() const {
			return children.size();
		}

		inline FBXNodePtr getChildNode(size_t _idx) const {
			if (_idx < 0 || _idx >= children.size()) return nullptr;
			return children[_idx];
		}

		inline size_t getPropertysSize() const {
			return propertys.size();
		}

		inline FBXPropertyPtr getProperty(size_t _idx) const {
			return propertys[_idx];
		}

	private:
		std::string name;                       // ノード名
		std::vector<FBXPropertyPtr> propertys;  // プロパティ
		FBXPerentNode perendtNode;              // 親ノード
		std::vector<FBXNodePtr> children;       // 子ノード
	};


	/*
	Mesh
		Vertex
		Index
		UV
		Normal
	Material
		Texture
	*/

	struct FbxGlobalSetting {
		int upAxis;
		int upAxisSing;
		int frontAxis;
		int frontAxisSign;
		int coordAxis;
		int coordAxisSign;
		int originalUpAxis;
		int originalUpAxisSign;
		double unitScaleFactor;
		double originalUnitScaleFactor;
		float ambientColor[3];
		std::string defaultCamera;
		// timeMode;
		// timeSpanStart;
		// timeSpanStop;
		double customFrameRate;
	};

	struct FbxGeometry {
		std::vector<float> vertices;
		std::vector<uint16_t> indexes;
		std::vector<int> edges;
	};

	struct FbxMaterial {
		float ambientColor[3];
		float ambientFactor;
		float diffuseColor[3];
		double bumpFactor;
		float specularColor[3];
		float specularFactor;
	};

	struct FbxTexture {
		std::string fileName;
	};

	struct FbxScene {
		std::vector<FbxGeometry> geometries;
		std::vector<FbxMaterial> materials;
		std::vector<FbxTexture> textures;
	};
}