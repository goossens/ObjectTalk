//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

//	Inspired by Joey de Vries' Learn OpenGL:
//	https://learnopengl.com/PBR/Lighting


#ifndef OT_PBR_GLSL
#define OT_PBR_GLSL


#include <bgfx_shader.glsl>


// constants
const float PI = 3.14159265359;

// PBR data
struct PBR {
	vec4 albedo;
	float metallic;
	float roughness;
	vec3 emissive;
	float ao;
	vec3 N; // normal
	vec3 V; // view direction
	vec3 L; // light direction
	vec3 H; // halfway vector
	vec3 directionalLightColor;
	float directionalLightAmbience;
};

// Normal Distribution Function (NDF)
float distributionGGX(vec3 N, vec3 H, float roughness) {
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;
	float denominator = max(NdotH2 * (a2 - 1.0) + 1.0, 0.0001);
	return a2 / (PI * denominator * denominator);
}

// geometry function
float geometrySchlickGGX(float NdotV, float roughness) {
	float r = (roughness + 1.0);
	float k = (r*r) / 8.0;
	return NdotV / (NdotV * (1.0 - k) + k);
}

float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = geometrySchlickGGX(NdotV, roughness);
	float ggx1 = geometrySchlickGGX(NdotL, roughness);
	return ggx1 * ggx2;
}

// fresnel equation
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
	return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

//	PBR calculation
vec4 applyPBR(PBR pbr) {
	vec3 albedo = pbr.albedo.rgb;

	// calculate reflectance
	vec3 F0 = mix(vec3_splat(0.04), albedo, pbr.metallic);

	// Cook-Torrance Bidirectional Reflective Distribution Function (BRDF)
	float NDF = distributionGGX(pbr.N, pbr.H, pbr.roughness);
	float G = geometrySmith(pbr.N, pbr.V, pbr.L, pbr.roughness);
	vec3 F = fresnelSchlick(max(dot(pbr.H, pbr.V), 0.0), F0);

	vec3 specular =
		(NDF * G * F) /
		(4.0 * max(dot(pbr.N, pbr.V), 0.0) * max(dot(pbr.N, pbr.L), 0.0) + 0.0001);

	vec3 kS = F;
	vec3 kD = (vec3_splat(1.0) - kS) * (1.0 - pbr.metallic);

	// determine directional light
	float NdotL = max(dot(pbr.N, pbr.L), 0.0);
	vec3 color = (kD * albedo / PI + specular) * pbr.directionalLightColor * NdotL;

	// add ambient light
	color += pbr.directionalLightAmbience * albedo * pbr.ao;

	// add emissive light
	color += pbr.emissive;

	// return result
	return vec4(color, pbr.albedo.a);
}


#endif
