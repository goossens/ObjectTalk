//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

#ifndef OT_UTILITIES_GLSL
#define OT_UTILITIES_GLSL


// determine the world coordinates from NDC coordinates
vec3 worldFromNdc(vec3 pos) {
	vec4 p = mul(u_invViewProj, vec4(pos, 1.0f));
	return p.xyz / p.w;
}

// determine the world coordinates from UV coordinates
vec3 worldFromUv(vec2 uv, float depth) {
#if BGFX_SHADER_LANGUAGE_GLSL
	return worldFromNdc(vec3(uv * 2.0 - 1.0, depth * 2.0 - 1.0));
#else
	return worldFromNdc(vec3(uv * 2.0 - 1.0, depth));
#endif
}

// determine NDC coordinates from world coordinates
vec3 NdcFromWorld(vec3 pos) {
	vec4 p = mul(u_viewProj, vec4(pos, 1.0));
	return p.xyz / pos.w;
}

// determine UV coordinates from world coordinates
vec3 uvFromWorld(vec3 pos) {
	vec3 p = NdcFromWorld(pos);
}

#endif
