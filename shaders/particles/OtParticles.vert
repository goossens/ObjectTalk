//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

#version 450 core

layout(location=0) in vec3 aPosition;
layout(location=1) in vec3 aPos;
layout(location=2) in float aAlpha;
layout(location=3) in vec2 aOffset1;
layout(location=4) in vec2 aOffset2;
layout(location=5) in float aScale;
layout(location=6) in float aRotate;
layout(location=7) in float aGrid;
layout(location=8) in float aBlend;

layout(location=0) out vec3 vData0;
layout(location=1) out vec3 vData1;

layout(std140, set=1, binding=0) uniform UBO {
	mat4 modelViewProjectionMatrix;
};


void main() {
	// determine world position of particle
	vec2 local = aPosition.xy * aScale;
	float cosR = cos(aRotate);
	float sinR = sin(aRotate);
	vec3 pos = aPos + vec3(cosR * local.x - sinR * local.y, sinR * local.x + cosR * local.y, 0.0);

	// determine texture coordinates
	vec2 uv = aPosition.xy + vec2(0.5);
	uv.y = 1.0 - uv.y;
	uv = uv / vec2(round(mod(aGrid, 100.0)), round(aGrid / 100.0));

	vec2 uv1 = aOffset1 + uv;
	vec2 uv2 = aOffset2 + uv;

	// pass to fragment shader
	vData0 = vec3(uv1, aAlpha);
	vData1 = vec3(uv2, aBlend);
	gl_Position = modelViewProjectionMatrix * vec4(pos, 1.0);
}
