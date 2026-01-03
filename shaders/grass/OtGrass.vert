//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

#version 450 core
#extension GL_GOOGLE_include_directive : require

#include "common.glsl"

layout(location=0) in vec3 aPosition;

layout(location=0) out vec3 vNormal;
layout(location=1) out vec3 vColor;
layout(location=2) out vec4 vData;

// main function
void main() {
	// determine vertex data
	Vertex vertex = getVertexData(aPosition);

	// get projected vertex position
	gl_Position = (viewProjectionMatrix * modelMatrix) * vec4(vertex.pos, 1.0);

	// get normal in world space
	vNormal = (modelMatrix * vec4(vertex.normal, 0.0)).xyz;

	// determine vertex color
	float variance = remap(vertex.hash2.w, -0.5, 0.5, 1.0 - colorVariation, 1.0);
	vColor = mix(baseColor.rgb * variance, tipColor.rgb * variance, vertex.y);

	// pass vertex data
	vData = vec4(vertex.x, 0.0, 0.0, 0.0);
}
