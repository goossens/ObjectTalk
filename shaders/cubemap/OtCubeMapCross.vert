//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

#version 450 core

layout(location=0) in vec3 aPosition;
layout(location=1) in vec3 aUv;

layout(location=0) out vec3 vUv;

layout(std140, set=1, binding=0) uniform UBO {
	mat4 viewProjectionMatrix;
};

void main() {
	vUv = aUv;
	gl_Position = viewProjectionMatrix * vec4(aPosition, 1.0);
}
