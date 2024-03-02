//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input a_position, a_normal, a_tangent, a_bitangent, a_texcoord0, i_data0, i_data1, i_data2, i_data3
$output v_position, v_normal, v_tangent, v_bitangent, v_texcoord0

#include <bgfx_shader.glsl>

void main() {
	mat4 model = u_model[0] * mtxFromCols(i_data0, i_data1, i_data2, i_data3);

	v_texcoord0 = a_texcoord0;
	v_position = mul(model, vec4(a_position, 1.0)).xyz;
	v_normal = mul(model, vec4(a_normal, 0.0)).xyz;
	v_tangent = mul(model, vec4(a_tangent, 0.0)).xyz;
	v_bitangent = mul(model, vec4(a_bitangent, 0.0)).xyz;

	gl_Position = mul(model * u_viewProj, vec4(a_position, 1.0));
}
