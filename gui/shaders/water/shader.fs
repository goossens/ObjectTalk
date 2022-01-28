//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_position_world, v_position_camera, v_position_screen, v_normal, v_tangent, v_bitangent

#include <bgfx.glsl>
#include <light.glsl>

uniform vec4 u_water[2];
#define u_time u_water[0].x
#define u_scale u_water[0].y
#define u_orientation u_water[0].z
#define u_water_color u_water[1]

SAMPLER2D(s_normals, 0);
SAMPLER2D(s_reflection, 1);
SAMPLER2D(s_refraction, 2);

void main() {
	mat3 tbn = mtxFromCols(v_tangent, v_normal, v_bitangent);

	vec2 uv = v_position_world.xz * u_scale;
	vec2 uv0 = uv / 103.0 + vec2(u_time / 17.0, u_time / 29.0);
	vec2 uv1 = uv / 107.0 - vec2(u_time / -19.0, u_time / 31.0);
	vec2 uv2 = uv / vec2(8907.0, 9803.0) + vec2(u_time / 101.0, u_time / 97.0);
	vec2 uv3 = uv / vec2(1091.0, 1027.0) - vec2(u_time / 109.0, u_time / -113.0);

	vec4 noise = (texture2D(s_normals, uv0) + texture2D(s_normals, uv1) + texture2D(s_normals, uv2) + texture2D(s_normals, uv3)) * 0.5 - 1.0;
	vec3 normal = normalize(mul(tbn, (noise.xzy * vec3(1.5, 1.0, 1.5))));

	vec2 fbuv = (v_position_screen.xy / v_position_screen.w) / 2.0 + 0.5;
	vec4 reflectionColor = texture2D(s_reflection, vec2(fbuv.x, fbuv.y * u_orientation));
	vec4 refractionColor = texture2D(s_refraction, vec2(fbuv.x, -fbuv.y * u_orientation));

	float reflectance = dot(vec3(0.0, 0.0, 1.0), normal);
	vec4 color = mix(reflectionColor, u_water_color, reflectance);
	gl_FragColor = applyLight(color, v_position_camera, normal);
}
