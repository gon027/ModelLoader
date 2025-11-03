#pragma once

namespace model {

	struct Vertex2 {
		float x;
		float y;

	public:
		Vertex2(float _x = 0.0f, float _y = 0.0f)
			: x{ _x }, y{ _y }
		{}
	};

	struct Vertex3 {
		float x;
		float y;
		float z;

	public:
		Vertex3(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f)
			: x{ _x }, y{ _y}, z{ _z }
		{}
	};

	struct Vertex4 {
		float x;
		float y;
		float z;
		float w;

	public:
		Vertex4(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f, float _w = 0.0f)
			: x{ _x }, y{ _y}, z{ _z}, w{ _w}
		{}
	};

}