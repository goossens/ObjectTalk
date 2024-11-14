//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_texcoord0

#include <bgfx_shader.glsl>
#include <constants.glsl>

// uniforms
uniform vec4 u_checkerboard[1];
#define u_repeat u_checkerboard[0].x

vec3 colorPalatte(float distance, float angle) {
	float theta = mod(3.0 + 3.0 * angle / PI + 1.5, 6.0);
	vec3 result = clamp(abs(mod(theta + vec3(0.0, 4.0, 2.0), 6.0) - 3.0) - 1.0, 0.0, 1.0);
	result = mix(vec3_splat(1.0), result, distance);
	return result;
}

void main() {
	/*
	vec2 uv = fract(v_texcoord0 * u_repeat) - 0.5;
	gl_FragColor = vec4(vec3_splat(step(uv.x * uv.y, 0.0)), 1.0);
	*/

	vec2 scale = (u_viewTexel.x > u_viewTexel.y) ? vec2(1.0, u_viewTexel.x / u_viewTexel.y) : vec2(u_viewTexel.y / u_viewTexel.x, 1.0) ;
	vec2 xy = (v_texcoord0 - 0.5) * 2.0 * scale;
	float distance = length(xy);
	float angle = atan2(xy.x, xy.y);

	vec3 color = (distance > 1.0) ? vec3_splat(0.0) : colorPalatte(distance, angle);
	color = mix(color, vec3_splat(0.0), 1.0 - smoothstep(0.0 ,0.01, distance));
	gl_FragColor = vec4(color, 1.0);
}
