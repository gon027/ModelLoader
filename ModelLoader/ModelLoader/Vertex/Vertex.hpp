#pragma once

namespace model {

	struct Vertex2 {
		float x;
		float y;
	};

	struct Vertex3 {
		float x;
		float y;
		float z;

		inline bool operator==(const Vertex3& _v) {
			return (x == _v.x) && (y == _v.y) && (z == _v.z);
		}

		inline bool operator!=(const Vertex3& _v) {
			return !(*this == _v);
		}
	};

	struct Vertex4 {
		float x;
		float y;
		float z;
		float w;
	};

}