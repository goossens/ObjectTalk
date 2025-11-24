//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

#version 450 core
#extension GL_GOOGLE_include_directive : require

#include "constants.glsl"
#include "pbrHelpers.glsl"

layout(location=0) in vec2 vUv;

layout(set=2, binding=0) uniform samplerCube inTexture;

layout(location=0) out vec4 side1Color;
layout(location=1) out vec4 side2Color;
layout(location=2) out vec4 side3Color;
layout(location=3) out vec4 side4Color;
layout(location=4) out vec4 side5Color;
layout(location=5) out vec4 side6Color;

layout(std140, set=3, binding=0) uniform UBO {
	float roughness;
	int size;
};

// From Karis, 2014
vec3 prefilterEnvMap(vec3 R) {
	// Isotropic approximation: we lose stretchy reflections :(
	vec3 N = R;
	vec3 V = R;
	vec3 prefilteredColor = vec3(0.0);
	float totalWeight = 0.0;

	const int numSamples = 64;

	for (int i = 0; i < numSamples; i++) {
		vec2 Xi = hammersley(i, numSamples);
		vec3 H = importanceSampleGGX(Xi, roughness, N);
		float VoH = dot(V, H);
		float NoH = VoH; // Since N = V in our approximation
		vec3 L = 2.0 * VoH * H - V;
		float NoL = clamp(dot(N, L), 0.0, 1.0);
		NoH = clamp(NoH, 0.0, 1.0);

		if (NoL > 0.0) {
			// Based off https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch20.html
			// Typically you'd have the following:
			// float pdf = D_GGX(NoH, roughness) * NoH / (4.0 * VoH);
			// but since V = N => VoH == NoH
			float pdf = D_GGX(NoH, roughness) / 4.0 + 0.001;
			// Solid angle of current sample -- bigger for less likely samples
			float omegaS = 1.0 / (float(numSamples) * pdf);
			// Solid angle of pixel
			float omegaP = 4.0 * PI / (6.0 * size * size);
			float mipLevel = roughness == 0.0 ? 0.0 : max(0.5 * log2(omegaS / omegaP), 0.0);
			prefilteredColor += textureLod(inTexture, L, mipLevel).rgb * NoL;
			totalWeight += NoL;
		}
	}

	return prefilteredColor / totalWeight;
}

vec4 processSide(vec3 N) {
	// Don't need to integrate for roughness == 0, since it's a perfect reflector
	return (roughness == 0.0) ? texture(inTexture, N, 0) : vec4(prefilterEnvMap(N), 1.0);
}

void main() {
	vec2 uv = 2.0 * vec2(vUv.x, 1.0 - vUv.y) - vec2(1.0);

	side1Color = processSide(normalize(vec3(1.0, uv.y, -uv.x)));
	side2Color = processSide(normalize(vec3(-1.0, uv.y, uv.x)));
	side3Color = processSide(normalize(vec3(uv.x, 1.0, -uv.y)));
	side4Color = processSide(normalize(vec3(uv.x, -1.0, uv.y)));
	side5Color = processSide(normalize(vec3(uv.x, uv.y, 1.0)));
	side6Color = processSide(normalize(vec3(-uv.x, uv.y, -1.0)));
}
