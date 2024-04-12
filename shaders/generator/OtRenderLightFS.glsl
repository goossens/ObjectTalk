//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

//	Inspired by: https://www.shadertoy.com/view/4dlGW2

$input v_texcoord0

#include <bgfx_shader.glsl>

// uniforms
uniform vec4 u_renderlight[2];
#define u_center u_renderlight[0].xy
#define u_size u_renderlight[0].zw
#define u_color u_renderlight[1].rgb

// functions
void main() {
	float distance = length(abs(v_texcoord0 - u_center) / u_size);
	float brightness = (1.0 - step(1.0, distance)) + (1.0 - smoothstep(1.0, 2.0, distance));
	gl_FragColor = vec4(u_color * brightness, 1.0);
}
