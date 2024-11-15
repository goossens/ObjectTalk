//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

// Based on https://en.wikipedia.org/wiki/Alpha_compositing

$input v_texcoord0

#include <bgfx_shader.glsl>

// uniforms
uniform vec4 u_rbgcurves[3];
#define u_mode int(u_rbgcurves[0].x)
#define u_black_level u_rbgcurves[1].rgb
#define u_white_level u_rbgcurves[2].rgb

// textures
SAMPLER2D(s_texture, 0);
SAMPLER2D(s_lut, 1);

void main() {
	vec4 color = texture2D(s_texture, v_texcoord0);

	if (u_mode == 0) {
		color = vec4(
			texture2D(s_lut, vec2(color.r, 0.5)).r,
			texture2D(s_lut, vec2(color.g, 0.5)).r,
			texture2D(s_lut, vec2(color.b, 0.5)).r,
			color.a);

	} else if (u_mode == 1) {
		color = vec4(texture2D(s_lut, vec2(color.r, 0.5)).r, color.g, color.b, color.a);

	} else if (u_mode == 2) {
		color = vec4(color.r, texture2D(s_lut, vec2(color.g, 0.5)).r, color.b, color.a);

	} else if (u_mode == 3) {
		color = vec4(color.r, color.g, texture2D(s_lut, vec2(color.b, 0.5)).r, color.a);

	} else if (u_mode == 4) {
		color = vec4(color.r, color.g, color.b, texture2D(s_lut, vec2(color.a, 0.5)).r);
	}

	float bwScale = 1.0 / (u_white_level - u_black_level);
	gl_FragColor = vec4((color.rgb - u_black_level) * bwScale, color.a);
}
