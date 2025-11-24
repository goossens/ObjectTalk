//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

#ifndef OT_UTILITIES_GLSL
#define OT_UTILITIES_GLSL

// convert point from clip to uv space
vec2 clipToUvSpace(vec3 pos) {
	return vec2(0.5 + 0.5 * pos.x, 0.5 - 0.5 * pos.y);
}

// convert point from uv to clip space
vec3 uvToClipSpace(vec2 uv, float depth) {
	return vec3(uv.x * 2.0 - 1.0, uv.y * -2.0 + 1.0, depth);
}

// convert point from uv to world space
vec3 uvToWorldSpace(vec2 uv, float depth, mat4 invViewProj) {
	vec4 p = invViewProj * vec4(uvToClipSpace(uv, depth), 1.0);
	return p.xyz / p.w;
}

// pre multiply color
vec4 preMultiplyAlpha(vec4 color) {
	return vec4(color.rgb * color.a, color.a);
}

// un pre multiply color
vec4 unPreMultiplyAlpha(vec4 color)  {
	if (color.a == 0.0) {
		return vec4(0.0);
	}

	return vec4(color.rgb / color.a, color.a);
}

// support functions
float inverseLerp(float value, float minValue, float maxValue) {
  return (value - minValue) / (maxValue - minValue);
}

float remap(float value, float inMin, float inMax, float outMin, float outMax) {
  return mix(outMin, outMax, inverseLerp(value, inMin, inMax));
}

float saturate(float x) {
  return clamp(x, 0.0, 1.0);
}

// create matrices
mat3 rotateY(float theta) {
	float c = cos(theta);
	float s = sin(theta);
	return mat3(vec3(c, 0.0, s), vec3(0.0, 1.0, 0.0), vec3(-s, 0.0, c));
}

mat3 rotateAxis(vec3 axis, float angle) {
	float s = sin(angle);
	float c = cos(angle);
	float oc = 1.0 - c;

	return mat3(
		oc * axis.x * axis.x + c,          oc * axis.x * axis.y - axis.z * s, oc * axis.z * axis.x + axis.y * s,
		oc * axis.x * axis.y + axis.z * s, oc * axis.y * axis.y + c,          oc * axis.y * axis.z - axis.x * s,
		oc * axis.z * axis.x - axis.y * s, oc * axis.y * axis.z + axis.x * s, oc * axis.z * axis.z + c);
}

#endif
