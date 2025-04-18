//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input a_position, a_texcoord0, a_indices, a_weight
$output v_texcoord0

#define BGFX_CONFIG_MAX_BONES 64
#include <bgfx_shader.glsl>

void main() {
	v_texcoord0 = a_texcoord0;

	vec4 pos = vec4(a_position, 1.0);

	vec4 worldPos =
		mul(u_model[int(a_indices.x)], pos) * a_weight.x +
		mul(u_model[int(a_indices.y)], pos) * a_weight.y +
		mul(u_model[int(a_indices.z)], pos) * a_weight.z +
		mul(u_model[int(a_indices.w)], pos) * a_weight.w;

	gl_Position = mul(u_viewProj, worldPos);
}
