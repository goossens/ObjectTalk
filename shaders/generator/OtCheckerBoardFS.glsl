//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

//	Derived from: https://github.com/patriciogonzalezvivo/lygia

$input v_texcoord0

#include <bgfx_shader.glsl>
#include <constants.glsl>

// uniforms
uniform vec4 u_checkerboard[3];
#define u_repeat u_checkerboard[0].x
#define u_black_color u_checkerboard[1]
#define u_white_color u_checkerboard[2]

void main() {
	vec2 uv = fract(v_texcoord0 * u_repeat) - 0.5;
	float ratio = step(uv.x * uv.y, 0.0);
	gl_FragColor = u_white_color * ratio + u_black_color * (1.0 - ratio);
}
