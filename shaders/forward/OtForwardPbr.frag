//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

#version 450 core
#extension GL_GOOGLE_include_directive : require

#define MATERIAL_UNIFORMS 0
#define MATERIAL_SAMPLERS 0
#include "material.glsl"

#define CLIPPING_UNIFORMS 1
#include "clipping.glsl"

#define LIGHTING_UNIFORMS 2
#define LIGHTING_SAMPLERS 5
#include "lighting.glsl"

#define SHADOW_UNIFORMS 3
#define SHADOW_SAMPLERS 8
#include "shadow.glsl"

#include "pbr.glsl"

layout(location=0) in vec3 vPosition;
layout(location=1) in vec3 vNormal;
layout(location=2) in vec3 vTangent;
layout(location=3) in vec3 vBitangent;
layout(location=4) in vec2 vUv;

layout(location=0) out vec4 fragColor;

// main function
void main() {
	// apply clipping plane
	clipAgainstPlane(vPosition);

	// determine UV coordinates
	vec2 uv = vUv * textureScale + textureOffset;

	// determine albedo
	vec4 albedo;

	if (hasAlbedoTexture) {
		albedo = texture(albedoTexture, uv) * albedoColor;

	} else {
		albedo = albedoColor;
	}

	// discard pixel if too transparent
	if (albedo.a < 0.3) {
		discard;
	}

	// material data
	Material material;
	material.albedo = toLinear(albedo.rgb);

	// determine normal
	if (hasNormalTexture) {
		vec3 tangent = normalize(vTangent);
		vec3 bitangent = normalize(vBitangent);
		vec3 normal = normalize(vNormal);
		mat3 TBN = mat3(tangent, bitangent, normal);
		material.normal = normalize(TBN * texture(normalTexture, uv).rgb * 2.0 - 1.0);

	} else {
		material.normal = normalize(vNormal);
	}

	// determine PBR parameters
	material.metallic = hasMetallicRoughnessTexture ? texture(metallicRoughnessTexture, uv).b * metallicFactor : metallicFactor;
	material.roughness = hasMetallicRoughnessTexture ? texture(metallicRoughnessTexture, uv).g * roughnessFactor : roughnessFactor;
	material.ao = hasAoTexture ? texture(aoTexture, uv).r * aoFactor : aoFactor;

	// determine view direction
	vec3 V = normalize(cameraPosition - vPosition);

	// total color
	vec3 color = vec3(0.0);

	// process directional light (if required)
	if (hasDirectionalLighting) {
		DirectionalLight light;
		light.L = normalize(directionalLightDirection);
		light.color = directionalLightColor;
		light.ambient = directionalLightAmbient;
		light.shadow = getShadow(vPosition, (viewTransform * vec4(vPosition, 1.0)).xyz, dot(material.normal, light.L));

		color += directionalLightPBR(material, light, V);
	}

	// process image based lighting (if required)
	if (hasImageBasedLighting) {
		color += imageBasedLightingPBR(material, V, iblEnvLevels, iblBrdfLut, iblIrradianceMap, iblEnvironmentMap);
	}

	// finalize color (tonemapping and gamma correction are done during post-processing)
	vec3 emissive = hasEmissiveTexture ? texture(emissiveTexture, uv).rgb * emissiveColor : emissiveColor;
	fragColor = vec4(color + toLinear(emissive), albedo.a);
}
