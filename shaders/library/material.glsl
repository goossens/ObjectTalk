//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

#ifndef OT_MATERIAL_GLSL
#define OT_MATERIAL_GLSL

// uniforms
layout(std140, set=3, binding=MATERIAL_UNIFORMS) uniform MaterialUniforms {
	vec4 albedoColor;
	vec3 emissiveColor;
	float textureScale;
	vec2 textureOffset;
	float metallicFactor;
	float roughnessFactor;
	float aoFactor;
	bool hasAlbedoTexture;
	bool hasMetallicRoughnessTexture;
	bool hasEmissiveTexture;
	bool hasAoTexture;
	bool hasNormalTexture;
};

// samplers
layout(set=2, binding=MATERIAL_SAMPLERS) uniform sampler2D albedoTexture;
layout(set=2, binding=MATERIAL_SAMPLERS + 1) uniform sampler2D metallicRoughnessTexture;
layout(set=2, binding=MATERIAL_SAMPLERS + 2) uniform sampler2D emissiveTexture;
layout(set=2, binding=MATERIAL_SAMPLERS + 3) uniform sampler2D aoTexture;
layout(set=2, binding=MATERIAL_SAMPLERS + 4) uniform sampler2D normalTexture;

#endif
