//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

#ifndef OT_CLIPPING_GLSL
#define OT_CLIPPING_GLSL

// uniforms
layout(std140, set=3, binding=CLIPPING_UNIFORMS) uniform ClippingUniforms {
	vec4 clippingPlane;
};

void clipAgainstPlane(vec3 position) {
	if (dot(clippingPlane, vec4(position, 1.0)) < 0.0) {
		discard;
	}
}

#endif
