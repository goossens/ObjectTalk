//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

#include "bgfx_compute.glsl"
#include "pbr_helpers.glsl"

#define PI 3.141592653589793

#define THREADS 8
#define NUM_SAMPLES 64u

uniform vec4 u_iblEnvironment;
#define u_roughness u_iblEnvironment.x
#define u_mipLevel u_iblEnvironment.y
#define u_size u_iblEnvironment.z

SAMPLERCUBE(s_cubemap, 0);
IMAGE2D_ARRAY_WO(s_target, rgba16f, 1);

// From Karis, 2014
vec3 prefilterEnvMap(float roughness, vec3 R, float imgSize){
	// Isotropic approximation: we lose stretchy reflections :(
	vec3 N = R;
	vec3 V = R;
	vec3 prefilteredColor = vec3_splat(0.0);
	float totalWeight = 0.0;

	for (uint i = 0u; i < NUM_SAMPLES; i++) {
		vec2 Xi = hammersley(i, NUM_SAMPLES);
		vec3 H = importanceSampleGGX(Xi, roughness, N);
		float VoH = dot(V, H);
		float NoH = VoH; // Since N = V in our approximation
		vec3 L = 2.0 * VoH * H - V;
		float NoL = saturate(dot(N, L));
		NoH = saturate(NoH);

		if (NoL > 0.0) {
			// Based off https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch20.html
			// Typically you'd have the following:
			// float pdf = D_GGX(NoH, roughness) * NoH / (4.0 * VoH);
			// but since V = N => VoH == NoH
			float pdf = D_GGX(NoH, roughness) / 4.0 + 0.001;
			// Solid angle of current sample -- bigger for less likely samples
			float omegaS = 1.0 / (float(NUM_SAMPLES) * pdf);
			// Solid angle  of pixel
			float omegaP = 4.0 * PI / (6.0 * imgSize * imgSize);
			float mipLevel = roughness == 0.0 ? 0.0 : max(0.5 * log2(omegaS / omegaP), 0.0);
			prefilteredColor += textureCubeLod(s_cubemap, L, mipLevel).rgb * NoL;
			totalWeight += NoL;
		}
	}

	return prefilteredColor / totalWeight;
}

NUM_THREADS(THREADS, THREADS, 6)
void main()
{
	float mipImageSize = u_size / pow(2.0, u_mipLevel);
	ivec3 globalId = ivec3(gl_GlobalInvocationID.xyz);

	if (globalId.x >= mipImageSize || globalId.y >= mipImageSize) {
		return;
	}

	vec3 R = normalize(toWorldCoords(globalId, mipImageSize));

	// Don't need to integrate for roughness == 0, since it's a perfect reflector
	if (u_roughness == 0.0) {
		vec4 color = textureCubeLod(s_cubemap, R, 0);
		imageStore(s_target, globalId, color);

	} else {
		vec3 color = prefilterEnvMap(u_roughness, R, u_size);
		imageStore(s_target, globalId, vec4(color, 1.0));
	}
}
