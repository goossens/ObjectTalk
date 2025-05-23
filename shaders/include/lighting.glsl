//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

//	Inspired by Joey de Vries' Learn OpenGL:
//	https://learnopengl.com/PBR/Lighting

#ifndef OT_LIGHTING_GLSL
#define OT_LIGHTING_GLSL


// uniforms
uniform vec4 u_lighting[4];
#define u_cameraPosition u_lighting[0].xyz
#define u_hasDirectionalLighting bool(u_lighting[0].w)
#define u_directionalLightDirection u_lighting[1].xyz
#define u_directionalLightColor u_lighting[2].xyz
#define u_directionalLightAmbience u_lighting[2].a
#define u_hasImageBasedLighting bool(u_lighting[3].x)
#define u_iblEnvLevels int(u_lighting[3].y)

// samplers
SAMPLER2D(s_iblBrdfLut, 5);
SAMPLERCUBE(s_iblIrradianceMap, 6);
SAMPLERCUBE(s_iblEnvironmentMap, 7);

// directional light data
struct DirectionalLight {
	vec3 L; // direction to light
	vec3 color;
	float ambience;
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
	float nom = saturate(1.0 - pow(distance / radius, 4.0));
	return nom * nom * distanceAttenuation(distance);
}


#endif
