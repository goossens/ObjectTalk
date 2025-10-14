//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

//	Inspired by Joey de Vries' Learn OpenGL:
//	https://learnopengl.com/PBR/Lighting

#ifndef OT_SHADOW_GLSL
#define OT_SHADOW_GLSL


#include <bgfx_shader.glsl>
#include <utilities.glsl>

uniform vec4 u_shadow[2];

#define u_shadowEnabled bool(u_shadow[0].x)
#define u_shadowTexelSize u_shadow[0].y

#define u_cascade0Distance u_shadow[1].x
#define u_cascade1Distance u_shadow[1].y
#define u_cascade2Distance u_shadow[1].z
#define u_cascade3Distance u_shadow[1].w

uniform mat4 u_shadowViewProjTransform[4];

SAMPLER2D(s_shadowMap0, 8);
SAMPLER2D(s_shadowMap1, 9);
SAMPLER2D(s_shadowMap2, 10);
SAMPLER2D(s_shadowMap3, 11);

// get hard shadow (on/off) at specified point
float hardShadow(sampler2D shadowmap, vec2 shadowCoord, float depth, float bias) {
	return step(depth, texture2D(shadowmap, shadowCoord).r + bias);
}

// get shadow value using Percentage-Closer Filtering (PCF)
float getCascadeShadow(sampler2D shadowmap, vec2 shadowCoord, float depth, float bias) {
	float shadow = 0.0;
	float offset = u_shadowTexelSize;

	shadow += hardShadow(shadowmap, shadowCoord + vec2(-1.5, -1.5) * offset, depth, bias);
	shadow += hardShadow(shadowmap, shadowCoord + vec2(-1.5, -0.5) * offset, depth, bias);
	shadow += hardShadow(shadowmap, shadowCoord + vec2(-1.5,  0.5) * offset, depth, bias);
	shadow += hardShadow(shadowmap, shadowCoord + vec2(-1.5,  1.5) * offset, depth, bias);

	shadow += hardShadow(shadowmap, shadowCoord + vec2(-0.5, -1.5) * offset, depth, bias);
	shadow += hardShadow(shadowmap, shadowCoord + vec2(-0.5, -0.5) * offset, depth, bias);
	shadow += hardShadow(shadowmap, shadowCoord + vec2(-0.5,  0.5) * offset, depth, bias);
	shadow += hardShadow(shadowmap, shadowCoord + vec2(-0.5,  1.5) * offset, depth, bias);

	shadow += hardShadow(shadowmap, shadowCoord + vec2(0.5, -1.5) * offset, depth, bias);
	shadow += hardShadow(shadowmap, shadowCoord + vec2(0.5, -0.5) * offset, depth, bias);
	shadow += hardShadow(shadowmap, shadowCoord + vec2(0.5,  0.5) * offset, depth, bias);
	shadow += hardShadow(shadowmap, shadowCoord + vec2(0.5,  1.5) * offset, depth, bias);

	shadow += hardShadow(shadowmap, shadowCoord + vec2(1.5, -1.5) * offset, depth, bias);
	shadow += hardShadow(shadowmap, shadowCoord + vec2(1.5, -0.5) * offset, depth, bias);
	shadow += hardShadow(shadowmap, shadowCoord + vec2(1.5,  0.5) * offset, depth, bias);
	shadow += hardShadow(shadowmap, shadowCoord + vec2(1.5,  1.5) * offset, depth, bias);

	return shadow / 16.0;
}

// get shadow value at specified position and distance from camera
float getShadow(vec3 wordPosition, vec3 viewPosition, float NdotL) {
	// see if we have shadowmaps
	if (u_shadowEnabled) {
		// determine shadow bias based on slope (shadowmap resolution is applied below)
		float bias = max(0.05 * (1.0 - NdotL), 0.005);

		// determine cascade and calculate shadow
		if (-viewPosition.z < u_cascade0Distance) {
			vec4 pos = mul(u_shadowViewProjTransform[0], vec4(wordPosition, 1.0));
			pos = pos / pos.w;
			return getCascadeShadow(s_shadowMap0, clipToUvSpace(pos.xyz), pos.z, bias * (1.0 / (u_cascade0Distance * 0.5)));

		} else if (-viewPosition.z < u_cascade1Distance) {
			vec4 pos = mul(u_shadowViewProjTransform[1], vec4(wordPosition, 1.0));
			pos = pos / pos.w;
			return getCascadeShadow(s_shadowMap1, clipToUvSpace(pos.xyz), pos.z, bias * (1.0 / (u_cascade1Distance * 0.5)));

		} else if (-viewPosition.z < u_cascade2Distance) {
			vec4 pos = mul(u_shadowViewProjTransform[2], vec4(wordPosition, 1.0));
			pos = pos / pos.w;
			return getCascadeShadow(s_shadowMap2, clipToUvSpace(pos.xyz), pos.z, bias * (1.0 / (u_cascade2Distance * 0.5)));

		} else if (-viewPosition.z < u_cascade3Distance) {
			vec4 pos = mul(u_shadowViewProjTransform[3], vec4(wordPosition, 1.0));
			pos = pos / pos.w;
			return getCascadeShadow(s_shadowMap3, clipToUvSpace(pos.xyz), pos.z, bias * (1.0 / (u_cascade3Distance * 0.5)));

		} else {
			return 1.0;
		}

	} else {
		return 1.0;
	}
}


#endif
