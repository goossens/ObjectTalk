//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

#version 450
#extension GL_GOOGLE_include_directive : require

#define LIGHTING_UNIFORMS 1
#define LIGHTING_SAMPLERS 4
#include "lighting.glsl"

#define SHADOW_UNIFORMS 2
#define SHADOW_SAMPLERS 7
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
	float distance;
	float depthFactor;
	float scale;
	float time;
	float metallic;
	float roughness;
	float ao;
	float reflectivity;
	bool refractanceFlag;
};

// texture samplers
layout(set=2, binding=0) uniform sampler2D waterNormalMapTexture;
layout(set=2, binding=1) uniform sampler2D reflectionTexture;
layout(set=2, binding=2) uniform sampler2D refractionTexture;
layout(set=2, binding=3) uniform sampler2D refractionDepthTexture;

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
	if (waterNdcPos.z > distance) {
		discard;
	}

	// get normal
	vec2 uv = waterWorldPos.xz * scale;
	vec2 uv1 = (uv / 103.0) + vec2(time / 17.0, time / 29.0);
	vec2 uv2 = (uv / 107.0) - vec2(time / -19.0, time / 31.0) + vec2(0.23);
	vec2 uv3 = (uv / vec2(897.0, 983.0)) + vec2(time / 101.0, time / 97.0) + vec2(0.51);
	vec2 uv4 = (uv / vec2(991.0, 877.0)) - vec2(time / 109.0, time / -113.0) + vec2(0.71);

	vec4 noise =
		texture(waterNormalMapTexture, uv1) +
		texture(waterNormalMapTexture, uv2) +
		texture(waterNormalMapTexture, uv3) +
		texture(waterNormalMapTexture, uv4);

	noise = noise * 0.5 - 1.0;

	float dist = length(cameraPosition - waterWorldPos);
	vec3 normal = normalize(noise.xzy * vec3(2.0, clamp(dist * 0.001, 1.0, 100.0), 2.0));

	// determine reflection and refraction colors
	vec2 refractionUv = gl_FragCoord.xy / size;
	vec2 reflectionUv = vec2(refractionUv.x, 1.0 - refractionUv.y);
	vec3 reflectionColor = texture(reflectionTexture, reflectionUv).rgb;
	vec3 refractionColor = waterColor.rgb * (refractanceFlag ? texture(refractionTexture, refractionUv).rgb : vec3(1.0));

	// determine view direction and water depth
	vec3 viewDirection = normalize(cameraPosition - waterWorldPos);

	// determine water depth
	float refractionDepth = texture(refractionDepthTexture, refractionUv).r;
	vec3 refractionWorldPos = uvToWorldSpace(refractionUv, refractionDepth, inverse(viewProjectionMatrix));
	float waterDepth = length(refractionWorldPos - waterWorldPos);

	// determine water color and transparency
	float refractiveFactor = pow(dot(viewDirection, normal), reflectivity);
	vec3 color = mix(reflectionColor, refractionColor, refractiveFactor);
	float alpha = clamp(waterDepth * depthFactor, 0.0, 1.0);

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
	fragColor = vec4(color, alpha);
	gl_FragDepth = waterNdcPos.z;
}
