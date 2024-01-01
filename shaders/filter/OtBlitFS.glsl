//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_texcoord0

#include <bgfx_shader.glsl>

SAMPLER2D(s_texture, 0);

uniform vec4 u_blit;
#define u_mult u_blit.x
#define u_alpha u_blit.y

void main() {
	gl_FragColor = texture2D(s_texture, v_texcoord0) * vec4(u_mult, u_mult, u_mult, u_mult * u_alpha);
}
