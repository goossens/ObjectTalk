//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

#version 450

layout(location=0) in vec2 aPosition;
layout(location=1) in vec2 aUv;

layout(location=0) out vec2 vPosition;
layout(location=1) out vec2 vUv;

layout(std140, set=1, binding=0) uniform UBO {
	float width;
	float height;
};

void main() {
	gl_Position = vec4(
		2.0 * aPosition.x / width - 1.0,
		1.0 - 2.0 * aPosition.y / height,
		0.0,
		1.0);

	vPosition = aPosition;
	vUv = aUv;
}
