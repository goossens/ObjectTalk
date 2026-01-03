//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

#version 450 core
#extension GL_GOOGLE_include_directive : require

#define CLIPPING_UNIFORMS 0
#include "clipping.glsl"

layout(location=0) in vec3 vPosition;
layout(location=1) in vec3 vNormal;

layout(location=0) out vec4 fragColor;
layout(location=1) out vec4 fragNormal;
layout(location=2) out vec4 fragPBR;
layout(location=3) out vec4 fragEmissive;

// uniforms
layout(std140, set=3, binding=1) uniform UBO {
	vec4 region1Color;
	vec4 region2Color;
	vec4 region3Color;
	vec4 region4Color;

	float hScale;
	float vScale;
	float vOffset;
	float heightMapSize;

	float region1TextureSize;
	float region2TextureSize;
	float region3TextureSize;
	float region4TextureSize;

	float region1TextureScale;
	float region2TextureScale;
	float region3TextureScale;
	float region4TextureScale;

	float region1Transition;
	float region2Transition;
	float region3Transition;

	float region1Overlap;
	float region2Overlap;
	float region3Overlap;
};

layout(set=2, binding=0) uniform sampler2D region1Texture;
layout(set=2, binding=1) uniform sampler2D region2Texture;
layout(set=2, binding=2) uniform sampler2D region3Texture;
layout(set=2, binding=3) uniform sampler2D region4Texture;

// main function
void main() {
	// apply clipping plane
	clipAgainstPlane(vPosition);

	// get parameters
	vec2 uv1 = fract(vPosition.xz / region1TextureSize * region1TextureScale);
	vec2 uv2 = fract(vPosition.xz / region2TextureSize * region2TextureScale);
	vec2 uv3 = fract(vPosition.xz / region3TextureSize * region3TextureScale);
	vec2 uv4 = fract(vPosition.xz / region4TextureSize * region4TextureScale);
	float height = (vPosition.y - vOffset) / vScale;

	// sample textures
	vec3 region1Albedo = bool(region1Color.a) ? texture(region1Texture, uv1).rgb : region1Color.rgb;
	vec3 region2Albedo = bool(region2Color.a) ? texture(region2Texture, uv2).rgb : region2Color.rgb;
	vec3 region3Albedo = bool(region3Color.a) ? texture(region3Texture, uv3).rgb : region3Color.rgb;
	vec3 region4Albedo = bool(region4Color.a) ? texture(region4Texture, uv4).rgb : region4Color.rgb;

	// adjust texture by slope
	float slope = abs(dot(vNormal, vec3(0.0, 1.0, 0.0)));
	float coef = 1.0 - smoothstep(0.5, 0.6, slope);
	region2Albedo = mix(region2Albedo, region3Albedo, coef);
	region4Albedo = mix(region4Albedo, region3Albedo, coef);

	// region 1
	vec3 albedo;

	if (height < region1Transition) {
		albedo = region1Albedo;

	// region 1 to region 2 transition
	} else if (height < region1Transition + region1Overlap) {
		albedo = mix(region1Albedo, region2Albedo, (height - region1Transition) / region1Overlap);

	// region 2
	} else if (height < region2Transition) {
		albedo = region2Albedo;

	// region 2 to region 3 transition
	} else if (height < region2Transition + region2Overlap) {
		albedo = mix(region2Albedo, region3Albedo, (height - region2Transition) / region2Overlap);

	// region 3
	} else if (height < region3Transition) {
		albedo = region3Albedo;

	// region 3 to region 4 transition
	} else if (height < region3Transition + region3Overlap) {
		albedo = mix(region3Albedo, region4Albedo, (height - region3Transition) / region3Overlap);

	// region 4
	} else {
		albedo = region4Albedo;
	}

	// store information in gbuffer
	fragColor = vec4(albedo, 1.0);
	fragNormal = vec4((normalize(vNormal) * 0.5 + 0.5), 1.0);
	fragPBR = vec4(0.0, 1.0, 1.0, 1.0);
	fragEmissive = vec4(0.0);
}
