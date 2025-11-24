//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

#version 450

layout(location=0) in vec3 aPosition;
layout(location=1) in vec3 aNormal;
layout(location=2) in vec3 aTangent;
layout(location=3) in vec3 aBitangent;
layout(location=4) in vec2 aUv;

layout(location=0) out vec3 vPosition;
layout(location=1) out vec3 vNormal;

layout(std140, set=1, binding=0) uniform UBO {
	mat4 modelViewProj;
	mat4 model;
};

void main() {
	vPosition = (model * vec4(aPosition, 1.0)).xyz;
	vNormal = (model * vec4(aNormal, 0.0)).xyz;
	gl_Position = modelViewProj * vec4(aPosition, 1.0);
}
