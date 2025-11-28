//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

#version 450 core
#extension GL_GOOGLE_include_directive : require

#define LIGHTING_UNIFORMS 1
#define LIGHTING_SAMPLERS 2
#include "lighting.glsl"

#define SHADOW_UNIFORMS 2
#define SHADOW_SAMPLERS 5
#include "shadow.glsl"

#include "pbr.glsl"
#include "utilities.glsl"

layout(location=0) in vec3 vNear;
layout(location=1) in vec3 vFar;

layout(location=0) out vec4 fragColor;

// uniforms
layout(std140, set=3, binding=0) uniform UBO {
	mat4 viewMatrix;
	mat4 viewProjectionMatrix;
	vec4 waterColor;
	vec2 size;
	float waterLevel;
	float farDistance;
	float scale;
	float time;
	float metallic;
	float roughness;
	float ao;
	float reflectivity;
};

// texture samplers
layout(set=2, binding=0) uniform sampler2D waterNormalMapTexture;
layout(set=2, binding=1) uniform sampler2D reflectionTexture;

// main program
void main() {
	// clip to visible plane
	float t = (waterLevel - vNear.y) / (vFar.y - vNear.y);

	if (t < 0.0) {
		discard;
	}

	// get the world and NDC positions
	vec3 waterWorldPos = vNear + t * (vFar - vNear);
	vec4 waterClipPos = viewProjectionMatrix * vec4(waterWorldPos, 1.0);
	vec3 waterNdcPos = waterClipPos.xyz / waterClipPos.w;

	// clip to distance
	if (waterNdcPos.z > farDistance) {
		discard;
	}

	// get water normal
	vec2 uv = waterWorldPos.xz / size * scale + vec2(-0.1, 0.1) * time;
	vec2 uv1 = uv + vec2(time / 17.0, time / 29.0);
	vec2 uv2 = uv - vec2(time / -19.0, time / 31.0);
	vec2 uv3 = uv + vec2(time / 101.0, time / 97.0);
	vec2 uv4 = uv - vec2(time / 109.0, time / -113.0);

	vec4 noise =
		texture(waterNormalMapTexture, uv1) +
		texture(waterNormalMapTexture, uv2) +
		texture(waterNormalMapTexture, uv3) +
		texture(waterNormalMapTexture, uv4);

	vec3 normal = normalize((noise.xzy * 0.5 - 1.0) * vec3(1.5, 1.0, 1.5));

	// determine view direction
	vec3 viewDirection = normalize(cameraPosition - waterWorldPos);

	// determine reflection
	vec2 reflectionUv = vec2(gl_FragCoord.xy) / size;
	reflectionUv = vec2(reflectionUv.x, 1.0 - reflectionUv.y);
	vec3 reflectionColor = texture(reflectionTexture, reflectionUv).rgb;

	// determine water color
	vec3 color = mix(waterColor.rgb, reflectionColor, pow(1.0 - dot(viewDirection, normal), reflectivity));

	// material data
	Material material;
	material.albedo = color;
	material.normal = normal;
	material.metallic = metallic;
	material.roughness = roughness;
	material.ao = ao;

	// light data
	DirectionalLight light;
	light.L = normalize(directionalLightDirection);
	light.color = directionalLightColor;
	light.ambient = directionalLightAmbient;

	// determine view direction
	vec3 V = normalize(cameraPosition - waterWorldPos);

	// total color
	color = vec3(0.0);

	// process directional light (if required)
	if (hasDirectionalLighting) {
		DirectionalLight light;
		light.L = normalize(directionalLightDirection);
		light.color = directionalLightColor;
		light.ambient = directionalLightAmbient;
		light.shadow = getShadow(waterWorldPos, (viewMatrix * vec4(waterWorldPos, 1.0)).xyz, dot(material.normal, light.L));

		color += directionalLightPBR(material, light, V);
	}

	// process image based lighting (if required)
	if (hasImageBasedLighting) {
		color += imageBasedLightingPBR(material, V, iblEnvLevels, iblBrdfLut, iblIrradianceMap, iblEnvironmentMap);
	}

	// set results
	fragColor = vec4(color, 1.0);
	gl_FragDepth = waterNdcPos.z;
}
