//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

#include "bgfx_compute.glsl"
#include "pbr_helpers.glsl"

#define THREADS 8
#define MIN_ROUGHNESS 0.045

IMAGE2D_WO(s_target, rg16f, 0);

// Karis 2014
vec2 integrateBRDF(float roughness, float NoV) {
	// normal always points along z-axis for the 2D lookup
	vec3 V = vec3(sqrt(1.0 - NoV * NoV), 0.0, NoV);
	vec3 N = vec3(0.0, 0.0, 1.0);

	float A = 0.0;
	float B = 0.0;
	uint numSamples = 1024;

	for (uint i = 0u; i < numSamples; i++) {
		vec2 Xi = hammersley(i, numSamples);
		//sSample microfacet direction
		vec3 H = importanceSampleGGX(Xi, roughness, N);

		// get the light direction
		vec3 L = 2.0 * dot(V, H) * H - V;
		float NoL = saturate(dot(N, L));
		float NoH = saturate(dot(N, H));
		float VoH = saturate(dot(V, H));

		if (NoL > 0.0) {
			// terms besides V are from the GGX PDF we're dividing by
			float V_pdf = V_SmithGGXCorrelated(NoV, NoL, roughness) * VoH * NoL / NoH;
			float Fc = pow(1.0 - VoH, 5.0);
			A += (1.0 - Fc) * V_pdf;
			B += Fc * V_pdf;
		}
	}

	return 4.0 * vec2(A, B) / float(numSamples);
}

NUM_THREADS(THREADS, THREADS, 1)
void main() {
	// normalized pixel coordinates to range [0, 1]
	vec2 uv = vec2(gl_GlobalInvocationID.xy) / vec2(imageSize(s_target).xy);

	// output to screen
	vec2 res = integrateBRDF(max(uv.y, MIN_ROUGHNESS), uv.x);

	// scale and bias for F0 (as per Karis 2014)
	imageStore(s_target, ivec2(gl_GlobalInvocationID.xy), vec4(res, 0.0, 0.0));
}
