//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

#ifndef OT_ALBEDO_GLSL
#define OT_ALBEDO_GLSL

// uniforms
layout(std140, set=3, binding=ALBEDO_UNIFORMS) uniform MaterialUniforms {
	vec4 albedoColor;
	vec2 textureOffset;
	float textureScale;
	bool hasAlbedoTexture;
};

// samplers
layout(set=2, binding=ALBEDO_SAMPLERS) uniform sampler2D albedoTexture;

#endif
