//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_texcoord0

#include <bgfx_shader.glsl>

SAMPLER2D(s_texture, 0);

uniform vec4 u_blur;
#define u_mult u_blur.x
#define u_alpha u_blur.y
#define u_blurX u_blur.z
#define u_blurY u_blur.a

void main() {
	vec4 color;
	color  = texture2D(s_texture, vec2(v_texcoord0.x - 4.0 * u_blurX, v_texcoord0.y - 4.0 * u_blurY)) * 0.05;
	color += texture2D(s_texture, vec2(v_texcoord0.x - 3.0 * u_blurX, v_texcoord0.y - 3.0 * u_blurY)) * 0.09;
	color += texture2D(s_texture, vec2(v_texcoord0.x - 2.0 * u_blurX, v_texcoord0.y - 2.0 * u_blurY)) * 0.12;
	color += texture2D(s_texture, vec2(v_texcoord0.x - 1.0 * u_blurX, v_texcoord0.y - 1.0 * u_blurY)) * 0.15;
	color += texture2D(s_texture, vec2(v_texcoord0.x + 0.0 * u_blurX, v_texcoord0.y + 0.0 * u_blurY)) * 0.16;
	color += texture2D(s_texture, vec2(v_texcoord0.x + 1.0 * u_blurX, v_texcoord0.y + 1.0 * u_blurY)) * 0.15;
	color += texture2D(s_texture, vec2(v_texcoord0.x + 2.0 * u_blurX, v_texcoord0.y + 2.0 * u_blurY)) * 0.12;
	color += texture2D(s_texture, vec2(v_texcoord0.x + 3.0 * u_blurX, v_texcoord0.y + 3.0 * u_blurY)) * 0.09;
	color += texture2D(s_texture, vec2(v_texcoord0.x + 4.0 * u_blurX, v_texcoord0.y + 4.0 * u_blurY)) * 0.05;
	gl_FragColor = color * vec4(u_mult, u_mult, u_mult, u_mult * u_alpha);
}
