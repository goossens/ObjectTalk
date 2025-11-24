//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

#ifndef OT_CLIP_GLSL
#define OT_CLIP_GLSL

// uniforms
layout(std140, set=3, binding=CLIP_UNIFORMS) uniform ClipUniforms {
	vec4 clipPlane;
};

void clipAgainstPlane(vec3 position) {
	if (dot(clipPlane, vec4(position, 1.0)) < 0.0) {
		discard;
	}
}

#endif
