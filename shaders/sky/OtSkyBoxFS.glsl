//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_position, v_texcoord0

#include <bgfx_shader.glsl>

// uniforms
uniform vec4 u_sky[3];
#define u_brightness u_sky[0].x
#define u_gamma u_sky[0].y

SAMPLERCUBE(s_skySampler, 0);

void main() {
	vec3 color = textureCube(s_skySampler, v_position).rgb;
	color = pow(color, vec3_splat(u_gamma));
	color = u_brightness * color;
	gl_FragColor = vec4(color, 1.0f);
}