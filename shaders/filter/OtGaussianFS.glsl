//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//	See https://www.rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling/ for more details


$input v_texcoord0

#include <bgfx_shader.glsl>
#include <utilities.glsl>

SAMPLER2D(s_texture, 0);

uniform vec4 u_gaussian;
#define u_direction u_gaussian.xy

void main() {
	vec2 off1 = vec2_splat(1.3846153846) * u_direction * u_viewTexel.xy;
	vec2 off2 = vec2_splat(3.2307692308) * u_direction * u_viewTexel.xy;

	vec4 color = preMultiplyAlpha(texture2D(s_texture, v_texcoord0)) * 0.2270270270;
	color += preMultiplyAlpha(texture2D(s_texture, v_texcoord0 + off1)) * 0.3162162162;
	color += preMultiplyAlpha(texture2D(s_texture, v_texcoord0 - off1)) * 0.3162162162;
	color += preMultiplyAlpha(texture2D(s_texture, v_texcoord0 + off2)) * 0.0702702703;
	color += preMultiplyAlpha(texture2D(s_texture, v_texcoord0 - off2)) * 0.0702702703;
	gl_FragColor = unPreMultiplyAlpha(color);
}
