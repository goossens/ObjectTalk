//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

#ifndef OT_LIGHTING_GLSL
#define OT_LIGHTING_GLSL

// uniforms
layout(std140, set=3, binding=LIGHTING_UNIFORMS) uniform LightingUniforms {
	vec3 cameraPosition;
	bool hasDirectionalLighting;
	vec3 directionalLightDirection;
	float directionalLightAmbient;
	vec3 directionalLightColor;
	bool hasImageBasedLighting;
	int iblEnvLevels;
};

// samplers
layout(set=2, binding=LIGHTING_SAMPLERS) uniform sampler2D iblBrdfLut;
layout(set=2, binding=LIGHTING_SAMPLERS + 1) uniform samplerCube iblIrradianceMap;
layout(set=2, binding=LIGHTING_SAMPLERS + 2) uniform samplerCube iblEnvironmentMap;

// directional light data
struct DirectionalLight {
	vec3 L; // direction to light
	vec3 color;
	float ambient;
	float shadow;
};

// point light data
struct PointLight {
	vec3 L; // direction to light
	float radius;
	vec3 color;
	float attenuation;
};

// inverse square falloff
float distanceAttenuation(float distance) {
	return 1.0 / max(distance * distance, 0.01 * 0.01);
}

// window function with smooth transition to 0
float smoothAttenuation(float distance, float radius) {
	float nom = clamp(1.0 - pow(distance / radius, 4.0), 0.0, 1.0);
	return nom * nom * distanceAttenuation(distance);
}

#endif
