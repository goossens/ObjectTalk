//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_texcoord0

#include <bgfx_shader.glsl>

uniform vec4 u_normalMapper;
#define u_normalStrength u_normalMapper.x
#define u_includeHeight bool(u_normalMapper.y)

SAMPLER2D(s_texture, 0);

void main() {
	// convert heightmap to normalmap using sobel filter
	float tl = texture2D(s_texture, v_texcoord0 + vec2(-u_viewTexel.x, -u_viewTexel.y)).r;
	float tc = texture2D(s_texture, v_texcoord0 + vec2(0.0, -u_viewTexel.y)).r;
	float tr = texture2D(s_texture, v_texcoord0 + vec2(u_viewTexel.x, -u_viewTexel.y)).r;
	float cl = texture2D(s_texture, v_texcoord0 + vec2(-u_viewTexel.x, 0.0)).r;
	float cc = texture2D(s_texture, v_texcoord0).r;
	float cr = texture2D(s_texture, v_texcoord0 + vec2(u_viewTexel.x, 0.0)).r;
	float bl = texture2D(s_texture, v_texcoord0 + vec2(-u_viewTexel.x, u_viewTexel.y)).r;
	float bc = texture2D(s_texture, v_texcoord0 + vec2(0.0, u_viewTexel.y)).r;
	float br = texture2D(s_texture, v_texcoord0 + vec2(u_viewTexel.x, u_viewTexel.y)).r;

	vec3 normal = normalize(vec3(
		(tr + (2.0 * cr) + br) - (tl + (2.0 * cl) + bl),
		(bl + (2.0 * bc) + br) - (tl + (2.0 * tc) + tr),
		1.0 / u_normalStrength));

	normal *= 0.5;
	normal += 0.5;

	gl_FragColor = vec4(normal, u_includeHeight ? cc : 1.0);
}
