#pragma once
#include "hmath.h"
struct BoundingShape {
	vec3* vertices;
	int num_vertices;
};

__declspec(dllimport) bool collides(BoundingShape* b1, BoundingShape* b2);
__declspec(dllimport) bool gjk_collides(BoundingShape* b1, BoundingShape* b2);
__declspec(dllimport) void transform_shape(BoundingShape* base, BoundingShape* b, mat4& m);
__declspec(dllimport) void DEBUG_gjk();