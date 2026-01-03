//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

//	Inspired by: https://vincenguyen.super.site/blog/blog-posts-1/how-to-render-pretty-water
//	Which was in inspired by https://www.youtube.com/playlist?list=PLRIWtICgwaX23jiqVByUs0bqhnalNTNZh

#version 450 core
#extension GL_GOOGLE_include_directive : require

#define LIGHTING_UNIFORMS 1
#define LIGHTING_SAMPLERS 5
#include "lighting.glsl"

#define SHADOW_UNIFORMS 2
#define SHADOW_SAMPLERS 8
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
	mat4 inverseProjectionMatrix;
	vec4 waterColor;
	vec2 size;
	float waterLevel;
	float farDistance;
	float depthFactor;
	float scale;
	float moveFactor;
	float metallic;
	float roughness;
	float ao;
	float reflectivity;
	bool usesRefractance;
};

// texture samplers
layout(set=2, binding=0) uniform sampler2D waterDuDvMapTexture;
layout(set=2, binding=1) uniform sampler2D waterNormalMapTexture;
layout(set=2, binding=2) uniform sampler2D reflectionTexture;
layout(set=2, binding=3) uniform sampler2D refractionTexture;
layout(set=2, binding=4) uniform sampler2D refractionDepthTexture;

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
	const float waveStrength = 0.015;
	vec2 textureCoords = waterWorldPos.xz / size * scale;
	vec2 distortedTexCoords = texture(waterDuDvMapTexture, vec2(textureCoords.x + moveFactor, textureCoords.y)).rg * 0.1;
	distortedTexCoords = textureCoords + vec2(distortedTexCoords.x, distortedTexCoords.y + moveFactor);
	vec2 totalDistortion = (texture(waterDuDvMapTexture, distortedTexCoords).rg * 2.0 - 1.0) * waveStrength;

	vec4 normalMapColor = texture(waterNormalMapTexture, distortedTexCoords);
	vec3 N = normalize(vec3(normalMapColor.r * 2.0 - 1.0, normalMapColor.b, normalMapColor.g * 2.0 - 1.0));

	// determine reflection and refraction colors
	vec2 refractionUv = gl_FragCoord.xy / size + totalDistortion;
	vec2 reflectionUv = vec2(refractionUv.x, 1.0 - refractionUv.y);
	vec3 reflectionColor = texture(reflectionTexture, reflectionUv).rgb;
	vec3 refractionColor = waterColor.rgb;
	float alpha = 1.0;

	// handle refraction (if required)
	if (usesRefractance) {
		// set refraction colors
		refractionColor *= texture(refractionTexture, refractionUv).rgb;

		// determine water depth
		float refractionDepth = texture(refractionDepthTexture, refractionUv).r;
		vec3 refractionWorldPos = uvToWorldSpace(refractionUv, refractionDepth, inverseProjectionMatrix);
		float waterDepth = length(waterWorldPos - refractionWorldPos);

		// determine water transparency
		alpha = clamp(waterDepth * depthFactor, 0.0, 1.0);
	}

	// determine view direction
	vec3 V = normalize(cameraPosition - waterWorldPos);

	// determine water color and transparency
	float refractiveFactor = clamp(pow(dot(V, N), reflectivity), 0.0, 1.0);
	vec3 color = mix(reflectionColor, refractionColor, refractiveFactor);

	// material data
	Material material;
	material.albedo = color;
	material.normal = N;
	material.metallic = metallic;
	material.roughness = roughness;
	material.ao = ao;

	// light data
	DirectionalLight light;
	light.L = normalize(directionalLightDirection);
	light.color = directionalLightColor;
	light.ambient = directionalLightAmbient;

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
