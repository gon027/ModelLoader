#include "FBXDataStruct.hpp"

namespace model::fbx {

	FBXProperty::FBXProperty()
		: type{}
		, value{}
	{
	}

	FBXProperty::FBXProperty(FBXPropertyType _type, FBXPropertyValue _value)
		: type{ _type }
		, value{ _value }
	{
	}

	void FBXProperty::setProperty(FBXPropertyType _type, FBXPropertyValue _value)
	{
		type = _type;
		value = _value;
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

	void FBXNode::addPropertys(const FBXProperty& _property)
	{
		propertys.push_back(_property);
	}

}


