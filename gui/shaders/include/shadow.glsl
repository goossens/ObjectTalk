//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

#ifndef OT_SHADOW_GLSL
#define OT_SHADOW_GLSL

#include <bgfx.glsl>

uniform vec4 u_shadow[5];

#define u_shadowEnabled bool(u_shadow[0].x)
#define u_shadowTexelSize u_shadow[0].yz
#define u_shadowMatrix mtxFromCols(u_shadow[1], u_shadow[2], u_shadow[3], u_shadow[4])

SAMPLER2D(s_shadowmap, 0);


//
//	hardShadow
//

float hardShadow(vec4 shadowCoord) {
	vec3 projCoords = (shadowCoord.xyz / shadowCoord.w);

	bool outside =
		any(lessThan(projCoords.xy, vec2_splat(0.0))) ||
		any(greaterThan(projCoords.xy, vec2_splat(1.0))) ||
		projCoords.z < 0.0 || projCoords.z > 1.0;

	if (outside) {
		return 1.0;
	}

	float occluder = texture2D(s_shadowmap, projCoords.xy).r;
	return step(projCoords.z, occluder);
}


//
//	getShadow
//

float getShadow(vec4 shadowCoord) {
	float shadow = 0.0;

	// Percentage-Closer Filtering (PCF)
	shadow += hardShadow(shadowCoord + vec4(vec2(-1.5, -1.5) * u_shadowTexelSize, 0.0, 0.0));
	shadow += hardShadow(shadowCoord + vec4(vec2(-1.5, -0.5) * u_shadowTexelSize, 0.0, 0.0));
	shadow += hardShadow(shadowCoord + vec4(vec2(-1.5,  0.5) * u_shadowTexelSize, 0.0, 0.0));
	shadow += hardShadow(shadowCoord + vec4(vec2(-1.5,  1.5) * u_shadowTexelSize, 0.0, 0.0));

	shadow += hardShadow(shadowCoord + vec4(vec2(-0.5, -1.5) * u_shadowTexelSize, 0.0, 0.0));
	shadow += hardShadow(shadowCoord + vec4(vec2(-0.5, -0.5) * u_shadowTexelSize, 0.0, 0.0));
	shadow += hardShadow(shadowCoord + vec4(vec2(-0.5,  0.5) * u_shadowTexelSize, 0.0, 0.0));
	shadow += hardShadow(shadowCoord + vec4(vec2(-0.5,  1.5) * u_shadowTexelSize, 0.0, 0.0));

	shadow += hardShadow(shadowCoord + vec4(vec2(0.5, -1.5) * u_shadowTexelSize, 0.0, 0.0));
	shadow += hardShadow(shadowCoord + vec4(vec2(0.5, -0.5) * u_shadowTexelSize, 0.0, 0.0));
	shadow += hardShadow(shadowCoord + vec4(vec2(0.5,  0.5) * u_shadowTexelSize, 0.0, 0.0));
	shadow += hardShadow(shadowCoord + vec4(vec2(0.5,  1.5) * u_shadowTexelSize, 0.0, 0.0));

	shadow += hardShadow(shadowCoord + vec4(vec2(1.5, -1.5) * u_shadowTexelSize, 0.0, 0.0));
	shadow += hardShadow(shadowCoord + vec4(vec2(1.5, -0.5) * u_shadowTexelSize, 0.0, 0.0));
	shadow += hardShadow(shadowCoord + vec4(vec2(1.5,  0.5) * u_shadowTexelSize, 0.0, 0.0));
	shadow += hardShadow(shadowCoord + vec4(vec2(1.5,  1.5) * u_shadowTexelSize, 0.0, 0.0));

	return shadow / 16.0;
}

#endif
