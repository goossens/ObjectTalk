//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

#ifndef OT_UTILITIES_GLSL
#define OT_UTILITIES_GLSL


// convert depth from depth buffer to linear space
float linearizeDepth(float depth, float near, float far) {
//	return near * far / (far - depth * (far - near));
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

// convert point from uv to view space
vec3 uvToViewSpace(vec2 uv, float depth) {
	vec3 clip = vec3(uv * 2.0 - 1.0, depthToClipSpace(depth));

#if !BGFX_SHADER_LANGUAGE_GLSL
	clip.y = -clip.y;
#endif

	return clipToViewSpace(clip);
}

// convert point from uv to world space
vec3 uvToWorldSpace(vec2 uv, float depth) {
	vec3 clip = vec3(uv * 2.0 - 1.0, depthToClipSpace(depth));

#if !BGFX_SHADER_LANGUAGE_GLSL
	clip.y = -clip.y;
#endif

	return clipToWorldSpace(clip);
}

// convert point from world to clip space
vec3 worldToClipSpace(vec3 pos) {
	vec4 p = mul(u_viewProj, vec4(pos, 1.0));
	return p.xyz / p.w;
}

// determine UV coordinates from world coordinates
vec2 worldSpaceToUv(vec3 pos) {
	vec3 p = worldToClipSpace(pos);
	vec2 uv = (p.xy + 1.0) * 0.5;

#if !BGFX_SHADER_LANGUAGE_GLSL
	uv.y = 1.0 - uv.y;
#endif

	return uv;
}


#endif
