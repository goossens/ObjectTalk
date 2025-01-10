//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input a_position, i_data0, i_data1, i_data2
$output v_texcoord0, v_texcoord1

#include <bgfx_shader.glsl>

void main() {
	// decode instance data
	vec3 pos = i_data0.xyz;
	float alpha = i_data0.w;
	vec2 offset1 = i_data1.xy;
	vec2 offset2 = i_data1.zw;
	float scale = i_data2.x;
	float rotate = i_data2.y;
	vec2 grid = vec2(round(i_data2.z % 100.0), round(i_data2.z / 100.0));
	float blend = i_data2.w;

	// determine world position of particle
	vec2 local = a_position.xy * scale;
	float cosR = cos(rotate);
	float sinR = sin(rotate);
	pos += vec3(cosR * local.x - sinR * local.y, sinR * local.x + cosR * local.y, 0.0);

	// determine texture coordinates
	vec2 uv = a_position + vec2_splat(0.5);
	uv.y = 1.0 - uv.y;
	uv = uv / grid;

	vec2 uv1 = offset1 + uv;
	vec2 uv2 = offset2 + uv;

	// pass to fragment shader
	v_texcoord0 = vec3(uv1, alpha);
	v_texcoord1 = vec3(uv2, blend);
	gl_Position = mul(u_modelViewProj, vec4(pos, 1.0));
}
