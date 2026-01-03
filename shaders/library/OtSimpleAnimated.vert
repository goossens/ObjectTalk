//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

#version 450 core

layout(location=0) in vec3 aPosition;
layout(location=1) in vec3 aNormal;
layout(location=2) in vec3 aTangent;
layout(location=3) in vec3 aBitangent;
layout(location=4) in vec2 aUv;
layout(location=5) in vec4 aIndices;
layout(location=6) in vec4 aWeights;

layout(location=0) out vec2 vUv;

layout(std140, set=1, binding=0) uniform UBO1 {
	mat4 viewProjectionMatrix;
	mat4 modelMatrix[64];
};

void main() {
	vUv = aUv;

	vec4 pos = vec4(aPosition, 1.0);

	vec4 worldPos =
		modelMatrix[int(aIndices.x)] * pos * aWeights.x +
		modelMatrix[int(aIndices.y)] * pos * aWeights.y +
		modelMatrix[int(aIndices.z)] * pos * aWeights.z +
		modelMatrix[int(aIndices.w)] * pos * aWeights.w;

	gl_Position = viewProjectionMatrix * worldPos;
}
