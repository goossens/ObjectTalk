//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input a_position, a_normal, a_tangent, a_texcoord0, i_data0, i_data1, i_data2, i_data3
$output v_position, v_normal, v_tangent, v_texcoord0, v_shadow

#include <bgfx.glsl>
#include <light.glsl>

void main() {
	mat4 model = mtxFromCols(i_data0, i_data1, i_data2, i_data3);
	vec4 position = mul(model, vec4(a_position, 1.0));
	v_position = position.xyz;
	v_normal = mul(model, vec4(a_normal, 0.0)).xyz;
	v_tangent = mul(u_model[0], vec4(a_tangent, 0.0)).xyz;
	v_texcoord0 = a_texcoord0;
	v_shadow = mul(u_shadow_matrix, position);
	gl_Position = mul(u_viewProj, position);
}
