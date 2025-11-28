//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

#version 450 core
#extension GL_GOOGLE_include_directive : require

#define ALBEDO_UNIFORMS 0
#define ALBEDO_SAMPLERS 0
#include "albedo.glsl"

layout(location=0) in vec2 vUv;

void main() {
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
}
