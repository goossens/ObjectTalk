//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

#version 450 core
#extension GL_GOOGLE_include_directive : require

#include "common.glsl"

layout(location=0) in vec3 aPosition;
layout(location=0) out vec2 vUv; // just a dummy to match the fragment shaders

// main function
void main() {
	// determine vertex data
	Vertex vertex = getVertexData(aPosition);

	// get projected vertex position
	gl_Position = (viewProjectionMatrix * modelMatrix) * vec4(vertex.pos, 1.0);
}
