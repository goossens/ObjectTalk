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
layout(location=2) out vec3 vTangent;
layout(location=3) out vec3 vBitangent;
layout(location=4) out vec2 vUv;

layout(std140, set=1, binding=0) uniform UBO {
	mat4 viewProjectionMatrix;
	mat4 modelMatrix;
};

void main() {
	vUv = aUv;
	vPosition = (modelMatrix * vec4(aPosition, 1.0)).xyz;
	vNormal = (modelMatrix * vec4(aNormal, 0.0)).xyz;
	vTangent = (modelMatrix * vec4(aTangent, 0.0)).xyz;
	vBitangent = (modelMatrix * vec4(aBitangent, 0.0)).xyz;
	gl_Position = viewProjectionMatrix * vec4(vPosition, 1.0);
}
