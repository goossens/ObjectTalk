//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

//	Inspired by Joey de Vries' Learn OpenGL:
//	https://learnopengl.com/PBR/Lighting

#ifndef OT_PBR_GLSL
#define OT_PBR_GLSL


#include <bgfx_shader.glsl>


// constants
#define PI 3.14159265359

// material data
struct Material {
	vec4 albedo;
	float metallic;
	float roughness;
	float ao;
	vec3 N; // surface normal
};

// directional light data
struct DirectionalLight {
	vec3 L; // direction to light
	vec3 V; // direction to view point
	vec3 color;
	float ambience;
};

// fresnel equations
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
	return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
	return F0 + (max(vec3_splat(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

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
	float k = (r * r) / 8.0;
	return NdotV / (NdotV * (1.0 - k) + k);
}

float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = geometrySchlickGGX(NdotV, roughness);
	float ggx1 = geometrySchlickGGX(NdotL, roughness);
	return ggx1 * ggx2;
}

//	PBR calculation for directional light
vec4 directionalLightPBR(Material material, DirectionalLight light, vec3 V) {
	// get material color
	vec3 albedo = material.albedo.rgb;

	// calculate surface reflection at zero incidence
	vec3 F0 = mix(vec3_splat(0.04), albedo, material.metallic);

	// calculate halfway vector between view direction and light direction
	vec3 H = normalize(V + light.L);

	// calculate the ratio between specular and diffuse reflection
	vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0); // Schlick Fresnel function

	// Cook-Torrance Microfacet Bidirectional Reflective Distribution (BRDF)
	float NDF = distributionGGX(material.N, H, material.roughness); // GGX normal distribution function
	float G = geometrySmith(material.N, V, light.L, material.roughness); // Smith Schlick geometry function

	vec3 specular =
		(NDF * G * F) /
		(4.0 * max(dot(material.N, V), 0.0) * max(dot(material.N, light.L), 0.0) + 0.0001);

	// calculate contribution to the reflectance equation
	vec3 kS = F;
	vec3 kD = (vec3_splat(1.0) - kS) * (1.0 - material.metallic);

	// determine outgoing radiance
	float NdotL = max(dot(material.N, light.L), 0.0);
	vec3 color = (kD * albedo / PI + specular) * light.color * NdotL;

	// add ambient light
	color += light.ambience * albedo * material.ao;

	// return result
	return vec4(color, material.albedo.a);
}


#endif
