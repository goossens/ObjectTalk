//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

#ifndef OT_LIGHT_GLSL
#define OT_LIGHT_GLSL


#include <material.glsl>


//
//	Light uniforms
//

#define BASE_UNIFORM_COUNT 11
#define POINT_SPOT_LIGHT_COUNT 4
#define UNIFORMS_PER_LIGHT 4

uniform vec4 u_light[BASE_UNIFORM_COUNT + POINT_SPOT_LIGHT_COUNT * UNIFORMS_PER_LIGHT];
#define u_camera_position u_light[0].xyz				// in worldspace
#define u_ambient_light_color u_light[1].rgb

#define u_directional_light_direction u_light[2].xyz	// from object to light (normalized)
#define u_directional_light_color u_light[3].rgb

#define u_fog_enabled bool(u_light[4].x)
#define u_fog_near u_light[4].y
#define u_fog_far u_light[4].z
#define u_fog_color u_light[5]

#define u_shadow_enabled bool(u_light[6].x)
#define u_shadow_texel_size u_light[6].yz
#define u_shadow_matrix mtxFromCols(u_light[7], u_light[8], u_light[9], u_light[10])

SAMPLER2D(s_shadowmap, 0);


//
//	Light structure
//

struct Light {
	bool enabled;
	vec3 position;				// in worldspace
	vec3 color;
	float attenuationConstant;
	float attenuationLinear;
	float attenuationQuadratic;
	vec3 coneDirection;			// from light
	float coneInner;			// as cos(angle)
	float coneOuter;			// as cos(angle) (zero for point light)
};


//
//	getAmbientLight
//

vec3 getAmbientLight(Material material) {
	return u_ambient_light_color * material.ambient;
}


//
//	getDirectionalLight
//

vec3 getDirectionalLight(Material material, vec3 position, vec3 normal) {
	// determine diffuse color
	float diffuseCoefficient = max(dot(normal, u_directional_light_direction), 0.0);
	vec3 diffuse = u_directional_light_color * material.diffuse * diffuseCoefficient;

	// determine specular color
	vec3 viewDirection = normalize(u_camera_position - position);
	vec3 halfwayDirection = normalize(u_directional_light_direction + viewDirection);
	float specularCoefficient = pow(max(dot(halfwayDirection, normal), 0.0), material.shininess);
	vec3 specular = u_directional_light_color * material.specular * specularCoefficient;

	// determine total light color
	return diffuse + specular;
}


//
//	getLight
//

Light getLight(int index) {
	Light light;
	int offset = BASE_UNIFORM_COUNT + index * UNIFORMS_PER_LIGHT;

	light.position = u_light[offset].xyz;
	light.enabled = bool(u_light[offset].w);
	light.color = u_light[offset + 1].rgb;
	light.attenuationConstant = u_light[offset + 2].x;
	light.attenuationLinear = u_light[offset + 2].y;
	light.attenuationQuadratic = u_light[offset + 2].z;
	light.coneInner = u_light[offset + 2].w;
	light.coneDirection = u_light[offset + 3].xyz;
	light.coneOuter = u_light[offset + 3].w;

	return light;
}


//
//	processLight
//

vec3 processLight(Light light, Material material, vec3 position, vec3 normal) {
	// determine normalized light direction and distance
	vec3 toLight = light.position - position;
		float distance = length(toLight);
	toLight = normalize(toLight);

	// determine diffuse color
	float diffuseCoefficient = max(dot(normal, toLight), 0.0);
	vec3 diffuse = light.color * material.diffuse * diffuseCoefficient;

	// determine specular color
	vec3 viewDirection = normalize(u_camera_position - position);
	vec3 halfwayDirection = normalize(toLight + viewDirection);
	float specularCoefficient = pow(max(dot(halfwayDirection, normal), 0.0), material.shininess);
	vec3 specular = light.color * material.specular * specularCoefficient;

	// determine light attenuation
	float attenuation = 1.0 / (
		light.attenuationConstant +
		light.attenuationLinear * distance +
		light.attenuationQuadratic * distance * distance);

	// determine cone effect
	float cone = 1.0;

	if (light.coneOuter > 0.0) {
		// only apply to spot lights
	    cone = clamp(
			(dot(toLight, normalize(-light.coneDirection)) - light.coneOuter) /
				(light.coneInner - light.coneOuter),
			0.0,
			1.0);
	}

	// determine total light color
	return cone * attenuation * (diffuse + specular);
}


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
	shadow += hardShadow(shadowCoord + vec4(vec2(-1.5, -1.5) * u_shadow_texel_size, 0.0, 0.0));
	shadow += hardShadow(shadowCoord + vec4(vec2(-1.5, -0.5) * u_shadow_texel_size, 0.0, 0.0));
	shadow += hardShadow(shadowCoord + vec4(vec2(-1.5,  0.5) * u_shadow_texel_size, 0.0, 0.0));
	shadow += hardShadow(shadowCoord + vec4(vec2(-1.5,  1.5) * u_shadow_texel_size, 0.0, 0.0));

	shadow += hardShadow(shadowCoord + vec4(vec2(-0.5, -1.5) * u_shadow_texel_size, 0.0, 0.0));
	shadow += hardShadow(shadowCoord + vec4(vec2(-0.5, -0.5) * u_shadow_texel_size, 0.0, 0.0));
	shadow += hardShadow(shadowCoord + vec4(vec2(-0.5,  0.5) * u_shadow_texel_size, 0.0, 0.0));
	shadow += hardShadow(shadowCoord + vec4(vec2(-0.5,  1.5) * u_shadow_texel_size, 0.0, 0.0));

	shadow += hardShadow(shadowCoord + vec4(vec2(0.5, -1.5) * u_shadow_texel_size, 0.0, 0.0));
	shadow += hardShadow(shadowCoord + vec4(vec2(0.5, -0.5) * u_shadow_texel_size, 0.0, 0.0));
	shadow += hardShadow(shadowCoord + vec4(vec2(0.5,  0.5) * u_shadow_texel_size, 0.0, 0.0));
	shadow += hardShadow(shadowCoord + vec4(vec2(0.5,  1.5) * u_shadow_texel_size, 0.0, 0.0));

	shadow += hardShadow(shadowCoord + vec4(vec2(1.5, -1.5) * u_shadow_texel_size, 0.0, 0.0));
	shadow += hardShadow(shadowCoord + vec4(vec2(1.5, -0.5) * u_shadow_texel_size, 0.0, 0.0));
	shadow += hardShadow(shadowCoord + vec4(vec2(1.5,  0.5) * u_shadow_texel_size, 0.0, 0.0));
	shadow += hardShadow(shadowCoord + vec4(vec2(1.5,  1.5) * u_shadow_texel_size, 0.0, 0.0));

	return shadow / 16.0;
}


//
//	processAllLights
//

vec3 processAllLights(Material material, vec3 position, vec3 normal, vec4 shadow) {
	vec3 totalLight = getDirectionalLight(material, position, normal);

	for (int i = 0; i < POINT_SPOT_LIGHT_COUNT; i++) {
		Light light = getLight(i);

		if (light.enabled) {
			totalLight += processLight(light, material, position, normal);
		}
	}

	float shade = u_shadow_enabled ? getShadow(shadow) : 1.0;
	return getAmbientLight(material) + shade * totalLight;
}


//
//	calculateFog
//

vec4 calculateFog(vec3 position, vec4 color) {
	float d = distance(u_camera_position, position);
	float fog_factor = (u_fog_far - d) / (u_fog_far - u_fog_near);
	return mix(u_fog_color, color, clamp(fog_factor, 0.0, 1.0));
}


//
//	applyLightAndFog
//

vec4 applyLightAndFog(vec4 color, Material material, vec3 position, vec3 normal, vec4 shadow) {
	vec3 light = processAllLights(material, position, normal, shadow);
	vec4 result = color * vec4(light, 1.0);
	return u_fog_enabled ? calculateFog(position, result) : result;
}

#endif
