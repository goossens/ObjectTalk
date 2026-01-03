//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

//	Inspired by Marie, How to make an infinite grid:
//	http://asliceofrendering.com/scene%20helper/2020/01/05/InfiniteGrid/

#version 450 core

layout(location=0) in vec3 aPosition;

layout(location=0) out vec3 vNear;
layout(location=1) out vec3 vFar;

layout(std140, set=1, binding=0) uniform UBO {
	mat4 inverseViewProjectionMatrix;
};

vec3 unproject(float x, float y, float z) {
	vec4 unprojectedPoint = inverseViewProjectionMatrix * vec4(x, y, z, 1.0);
	return unprojectedPoint.xyz / unprojectedPoint.w;
}

void main() {
	vNear = unproject(aPosition.x, aPosition.y, 0.0);
	vFar = unproject(aPosition.x, aPosition.y, 1.0);
	gl_Position = vec4(aPosition, 1.0);
}
