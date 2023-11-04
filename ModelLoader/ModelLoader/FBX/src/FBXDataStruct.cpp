#include "../FBXDataStruct.hpp"

namespace model::fbx {

	namespace internal {

		FBXNode::FBXNodePtr findChildNodeByName(FBXNode::FBXNodePtr _node, const std::string _name) {
			if (_node->getNodeName() == _name) {
				return _node;
			}

			for (size_t i{ 0 }; i < _node->getChildrenSize(); ++i) {
				auto node = _node->getChildNode(i);
				if (node->getNodeName() == _name) {
					return node;
				}
			}

			return nullptr;
		}

	}

	FBXNode::FBXNode()
		: name()
		, propertys()
		, perendtNode{ }
		, children()
	{
	}

	FBXNode::FBXNode(const std::string& _name)
		: name(_name)
		, propertys()
		, perendtNode{ }
		, children()
	{
	}

	void FBXNode::setName(const std::string& _name)
	{
		name = _name;
	}

	void FBXNode::addNode(FBXNodePtr _node)
	{
		children.push_back(_node);
	}

	// void FBXNode::setPerentNode(FBXPerentNode _perentNode)
	// {
	// }

	void FBXNode::addPropertys(FBXPropertyPtr _property)
	{
		propertys.push_back(_property);
	}

	FBXNode::FBXNodePtr FBXNode::findNode(const std::string _name)
	{
		for (auto& _node : children) {
			auto node = internal::findChildNodeByName(_node, _name);
			if (!node) {
				continue;
			}

			if (node->getNodeName() == _name) {
				return node;
			}
		}

		return nullptr;
	}

	FBXPropertyPtr FBXNode::findPropertyForChildren(const std::string& _name)
	{
		for (auto child : children) {
			if (child->getPropertysSize() == 0) {
				return nullptr;
			}

			auto cProp = child->getProperty(0);
			if (!cProp) {
				return nullptr;
			}

			auto cPropName = dynamic_pointer_cast<FBXStringProperty>(cProp)->getValue();
			if (cPropName == _name) {
				return child->getProperty(child->getPropertysSize() - 1);
			}

		}

		return nullptr;
	}

	FBXPropertyPtr FBXNode::findProperty(const std::string& _name)
	{
		auto prop = propertys[0];
		if (!prop) {
			return nullptr;
		}

		auto type = propertys[0]->getType();
		if (type != FBXPropertyType::String) {
			return nullptr;
		}

		auto propName = dynamic_pointer_cast<FBXStringProperty>(prop)->getValue();
		if (propName == _name) {
			return propertys[getPropertysSize() - 1];
		}

		return nullptr;
	}

}


