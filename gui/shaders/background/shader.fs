//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_texcoord0

#include <bgfx.glsl>

uniform vec4 u_background[2];
#define u_background_textured bool(u_background[0].x)
#define u_background_color u_background[1]

SAMPLER2D(s_texture, 0);

void main() {
	if (u_background_textured) {
		gl_FragColor = texture2D(s_texture, v_texcoord0);

	} else {
		gl_FragColor = u_background_color;
	}
}
