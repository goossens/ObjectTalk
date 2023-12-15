//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

#ifndef OT_CLIP_GLSL
#define OT_CLIP_GLSL


uniform vec4 u_clip;

void clipPlane(vec3 position) {
	if (dot(u_clip, vec4(position, 1.0)) < 0.0) {
		discard;
	}
}


#endif