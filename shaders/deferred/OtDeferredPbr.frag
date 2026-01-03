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

layout(location=0) in vec3 vPosition;
layout(location=1) in vec3 vNormal;
layout(location=2) in vec3 vTangent;
layout(location=3) in vec3 vBitangent;
layout(location=4) in vec2 vUv;

layout(location=0) out vec4 fragColor;
layout(location=1) out vec4 fragNormal;
layout(location=2) out vec4 fragPBR;
layout(location=3) out vec4 fragEmissive;

// main function
void main() {
	// apply clipping plane
	clipAgainstPlane(vPosition);

	// determine UV coordinates
	vec2 uv = vUv * textureScale + textureOffset;

	// determine albedo
	vec3 albedo = albedoColor.rgb;

	if (hasAlbedoTexture) {
		albedo = texture(albedoTexture, uv).rgb * albedo;
	}

	// determine normal
	vec3 normal = normalize(vNormal);

	if (hasNormalTexture) {
		// apply normal texture
		vec3 tangent = normalize(vTangent);
		vec3 bitangent = normalize(vBitangent);
		mat3 TBN = mat3(tangent, bitangent, normal);
		normal = normalize(TBN * (texture(normalTexture, uv).rgb * 2.0 - 1.0));
	}

	// determine PBR parameters
	float metallic = (hasMetallicRoughnessTexture ? texture(metallicRoughnessTexture, uv).b : 1.0) * metallicFactor;
	float roughness = (hasMetallicRoughnessTexture ? texture(metallicRoughnessTexture, uv).g : 1.0) * roughnessFactor;
	vec3 emissive = (hasEmissiveTexture ? texture(emissiveTexture, uv).rgb : vec3(1.0)) * emissiveColor;
	float ao = (hasAoTexture ? texture(aoTexture, uv).r : 1.0) * aoFactor;

	// store information in gbuffer
	fragColor = vec4(albedo, 1.0);
	fragNormal = vec4((normal * 0.5 + 0.5), 1.0);
	fragPBR = vec4(metallic, roughness, ao, 1.0);
	fragEmissive = vec4(emissive, 1.0);
}
