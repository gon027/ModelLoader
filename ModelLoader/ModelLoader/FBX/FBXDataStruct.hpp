#pragma once

#include <string>
#include <vector>
#include <memory>
#include <variant>

/*
  Header
  GlobalSettings
  Documents
  References
  Definitions
  Objects
  Connections
  Takes
*/

namespace model::fbx {

	enum class FBXPropertyType : char {
		String    = 'S',
		Int16     = 'Y',
		Int32     = 'I',
		Int64     = 'L',
		Float     = 'F',
		Double    = 'D',
		Bool      = 'C',
		VecUInt8  = 'R',
		VecInt32  = 'i',
		VecInt64  = 'l',
		VecFloat  = 'f',
		VecDouble = 'd',
		VecBool   = 'b',
	};

	using FBXPropertyValue = std::variant<
		std::string,
		short,
		int,
		long long,
		float,
		double,
		bool,
		std::vector<int>,
		std::vector<long long>,
		std::vector<float>,
		std::vector<double>,
		std::vector<char>
	>;

	class FBXProperty {
	public:
		FBXProperty();
		FBXProperty(FBXPropertyType _type, FBXPropertyValue _value);
		~FBXProperty() = default;

		void setProperty(FBXPropertyType _type, FBXPropertyValue _value);

		void setType(FBXPropertyType _type) {
			type = _type;
		}

		inline FBXPropertyType getType() const {
			return type;
		}

		inline FBXPropertyValue getValue() const {
			return value;
		}

	private:
		FBXPropertyType type;
		FBXPropertyValue value;
	};

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

		void addPropertys(const FBXProperty& _property);

		inline std::string getNodeName() const {
			return name;
		}

		inline size_t getChildrenSize() const {
			return children.size();
		}

		inline size_t getPropertysSize() const {
			return propertys.size();
		}

	private:
		std::string name;                    // ノード名
		std::vector<FBXProperty> propertys;  // プロパティ
		FBXPerentNode perendtNode;           // 親ノード
		std::vector<FBXNodePtr> children;    // 子ノード
	};

}