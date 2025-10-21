//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

#ifndef OT_ALBEDO_GLSL
#define OT_ALBEDO_GLSL


// uniforms
uniform vec4 u_albedo[2];
#define u_color u_albedo[0]
#define u_hasTexture bool(u_albedo[1].x)
#define u_scale u_albedo[1].y

SAMPLER2D(s_albedoTexture, 0);

#endif
