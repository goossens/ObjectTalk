//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "assimp/scene.h"
#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"


//
//	Helper functions
//

inline glm::vec2 toVec2(const aiVector3D& v) {
	return glm::vec2(v.x, v.y);
}

inline glm::vec3 toVec3(const aiVector3D& v) {
	return glm::vec3(v.x, v.y, v.z);
}

inline glm::mat4 toMat4(const aiMatrix4x4& mat) {
	glm::mat4 m;

	for (int y = 0; y < 4; y++) {
		for (int x = 0; x < 4; x++) {
			m[x][y] = mat[y][x];
		}
	}

	return m;
}

inline glm::quat toQuat(const aiQuaternion& quat) {
	glm::quat q;
	q.x = quat.x;
	q.y = quat.y;
	q.z = quat.z;
	q.w = quat.w;
	return q;
}
