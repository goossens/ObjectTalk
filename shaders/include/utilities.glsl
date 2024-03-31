//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

#ifndef OT_UTILITIES_GLSL
#define OT_UTILITIES_GLSL

#include <bgfx_shader.glsl>

// convert depth to linear space
float linearizeDepth(float depth, float near, float far) {
	return (2.0 * near) / (far + near - depth * (far - near));
}

// convert depth from depth buffer to clip space depth
float depthToClipSpace(float depth) {
#if BGFX_SHADER_LANGUAGE_GLSL
	return depth * 2.0 - 1.0;
#else
	return depth;
#endif
}

// convert point from clip to uv space
vec2 clipToUvSpace(vec3 pos) {
#if BGFX_SHADER_LANGUAGE_GLSL
	return (pos.xy + 1.0) * 0.5;
#else
	return vec2((pos.x + 1.0) * 0.5, 1.0 - ((pos.y + 1.0) * 0.5));
#endif
}

// convert point from clip to view space
vec3 clipToViewSpace(vec3 pos) {
	vec4 p = mul(u_invProj, vec4(pos, 1.0f));
	return p.xyz / p.w;
}

// convert point from clip to world space
vec3 clipToWorldSpace(vec3 pos) {
	vec4 p = mul(u_invViewProj, vec4(pos, 1.0f));
	return p.xyz / p.w;
}

// convert point from uv to clip space
vec3 uvToClipSpace(vec2 uv, float depth) {
#if BGFX_SHADER_LANGUAGE_GLSL
	return vec3(uv * 2.0 - 1.0, depthToClipSpace(depth));
#else
	return vec3(uv.x * 2.0 - 1.0, uv.y * -2.0 + 1.0, depthToClipSpace(depth));
#endif
}

// convert point from uv to view space
vec3 uvToViewSpace(vec2 uv, float depth) {
	return clipToViewSpace(uvToClipSpace(uv, depth));
}

// convert point from uv to world space
vec3 uvToWorldSpace(vec2 uv, float depth) {
	return clipToWorldSpace(uvToClipSpace(uv, depth));
}

// convert point from world to clip space
vec3 worldToClipSpace(vec3 pos) {
	vec4 p = mul(u_viewProj, vec4(pos, 1.0));
	return p.xyz / p.w;
}

// determine UV coordinates from world coordinates
vec2 worldSpaceToUv(vec3 pos) {
	return clipToUvSpace(worldToClipSpace(pos));
}


#endif
