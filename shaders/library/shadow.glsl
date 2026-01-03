//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

//	Inspired by Joey de Vries' Learn OpenGL:
//	https://learnopengl.com/PBR/Lighting

#ifndef OT_SHADOW_GLSL
#define OT_SHADOW_GLSL

#include "utilities.glsl"

// uniforms
layout(std140, set=3, binding=SHADOW_UNIFORMS) uniform ShadowUniforms {
	mat4 viewTransform;
	mat4 shadowViewProjTransform0;
	mat4 shadowViewProjTransform1;
	mat4 shadowViewProjTransform2;
	mat4 shadowViewProjTransform3;
	float cascadeDistance0;
	float cascadeDistance1;
	float cascadeDistance2;
	float cascadeDistance3;
	float shadowTexelSize;
	bool shadowEnabled;
};

layout(set=2, binding=SHADOW_SAMPLERS) uniform sampler2D shadowMap0;
layout(set=2, binding=SHADOW_SAMPLERS + 1) uniform sampler2D shadowMap1;
layout(set=2, binding=SHADOW_SAMPLERS + 2) uniform sampler2D shadowMap2;
layout(set=2, binding=SHADOW_SAMPLERS + 3) uniform sampler2D shadowMap3;

// get hard shadow (on/off) at specified point
float hardShadow(sampler2D shadowmap, vec2 shadowCoord, float depth, float bias) {
	return step(depth, texture(shadowmap, shadowCoord).r + bias);
}

// get shadow value using Percentage-Closer Filtering (PCF)
float getCascadeShadow(sampler2D shadowmap, vec2 shadowCoord, float depth, float bias) {
	float shadow = 0.0;
	float offset = shadowTexelSize;

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
	if (shadowEnabled) {
		// determine shadow bias based on slope (shadowmap resolution is applied below)
		float bias = max(0.05 * (1.0 - NdotL), 0.005);

		// determine cascade and calculate shadow
		if (-viewPosition.z < cascadeDistance0) {
			vec4 pos = shadowViewProjTransform0 * vec4(wordPosition, 1.0);
			vec2 uv = clipToUvSpace((pos / pos.w).xyz);
			return getCascadeShadow(shadowMap0, uv, pos.z, bias * (1.0 / (cascadeDistance0 * 0.5)));

		} else if (-viewPosition.z < cascadeDistance1) {
			vec4 pos = shadowViewProjTransform1 * vec4(wordPosition, 1.0);
			vec2 uv = clipToUvSpace((pos / pos.w).xyz);
			return getCascadeShadow(shadowMap1, uv, pos.z, bias * (1.0 / (cascadeDistance1 * 0.5)));

		} else if (-viewPosition.z < cascadeDistance2) {
			vec4 pos = shadowViewProjTransform2 * vec4(wordPosition, 1.0);
			vec2 uv = clipToUvSpace((pos / pos.w).xyz);
			return getCascadeShadow(shadowMap2, uv, pos.z, bias * (1.0 / (cascadeDistance2 * 0.5)));

		} else if (-viewPosition.z < cascadeDistance3) {
			vec4 pos = shadowViewProjTransform3 * vec4(wordPosition, 1.0);
			vec2 uv = clipToUvSpace((pos / pos.w).xyz);
			return getCascadeShadow(shadowMap3, uv, pos.z, bias * (1.0 / (cascadeDistance3 * 0.5)));

		} else {
			return 1.0;
		}

	} else {
		return 1.0;
	}
}

#endif
