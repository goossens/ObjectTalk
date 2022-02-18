//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

#ifndef OT_SHADOW_GLSL
#define OT_SHADOW_GLSL

uniform mat4 u_shadowMatrix;

SAMPLER2D(s_shadowmap, 0);

#endif
