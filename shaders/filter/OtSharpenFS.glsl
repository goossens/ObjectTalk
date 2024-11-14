//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_texcoord0

#include <bgfx_shader.glsl>

uniform vec4 u_sharpen[1];
#define u_strength u_sharpen[0].x

SAMPLER2D(s_texture, 0);

void main() {
	vec3 sum = vec3_splat(0.0);

	for (int i = 0; i < 2; i++) {
		float size = (float(i) + 1.0) * u_strength;
		sum += -1.0 * texture2D(s_texture, v_texcoord0 + vec2(-1.0,  0.0) * u_viewTexel.xy * size).rgb;
		sum += -1.0 * texture2D(s_texture, v_texcoord0 + vec2( 0.0, -1.0) * u_viewTexel.xy * size).rgb;
		sum +=  5.0 * texture2D(s_texture, v_texcoord0 + vec2( 0.0,  0.0) * u_viewTexel.xy * size).rgb;
		sum += -1.0 * texture2D(s_texture, v_texcoord0 + vec2( 0.0,  1.0) * u_viewTexel.xy * size).rgb;
		sum += -1.0 * texture2D(s_texture, v_texcoord0 + vec2( 1.0,  0.0) * u_viewTexel.xy * size).rgb;
	}

	gl_FragColor = vec4(sum / 2.0, 1.0);
}
