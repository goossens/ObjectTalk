//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

#version 450 core

layout(location=0) in vec3 aPosition;
layout(location=1) in vec3 aLightPos;
layout(location=2) in vec3 aColor;
layout(location=3) in float aRadius;

layout(location=0) out vec3 vLightPos;
layout(location=1) out vec4 vColor;

layout(std140, set=1, binding=0) uniform UBO {
	mat4 viewProjectionMatrix;
};

void main() {
	// gather information for fragment shader
	vLightPos = aLightPos;
	vColor = vec4(aColor, aRadius);
	gl_Position = viewProjectionMatrix * vec4(aLightPos + aPosition * aRadius, 1.0);
}
