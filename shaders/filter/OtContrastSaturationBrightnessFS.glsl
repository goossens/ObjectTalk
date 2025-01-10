//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

// based on https://github.com/SableRaf/Filters4Processing/blob/master/sketches/ContrastSaturationBrightness/data/ContrastSaturationBrightness.glsl

$input v_texcoord0

#include <bgfx_shader.glsl>

uniform vec4 u_csb[1];
#define u_contrast u_csb[0].x
#define u_saturation u_csb[0].y
#define u_brightness u_csb[0].z

SAMPLER2D(s_texture, 0);

void main() {
	vec3 LumCoeff = vec3(0.2125, 0.7154, 0.0721);
	vec3 AvgLumin  = vec3(0.5, 0.5, 0.5);

	vec3 brtColor  = texture2D(s_texture, v_texcoord0).rgb * u_brightness;
	vec3 intensity = vec3_splat(dot(brtColor, LumCoeff));
	vec3 satColor  = mix(intensity, brtColor, u_saturation);
	vec3 conColor  = mix(AvgLumin, satColor, u_contrast);

	gl_FragColor = vec4(conColor, 1.0);
}
