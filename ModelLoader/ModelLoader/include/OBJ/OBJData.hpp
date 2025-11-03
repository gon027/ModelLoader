#pragma once

#include "../Vertex/Vertex.hpp"

namespace model::obj {

	// https://asura.iaigiri.com/OpenGL/gl15_2.html

	// ’¸“_(v)
	// –@ü(vn)
	// –Êî•ñ
	// Ş¿

	struct ObjVertex {
		model::Vertex3 position;
		model::Vertex2 texcoord;
		model::Vertex3 normal;

	public:
		ObjVertex()
			: position{}, texcoord{}, normal{}
		{}
	};

	struct ObjMaterial {

	};
}