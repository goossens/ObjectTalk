//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input a_position, a_normal, i_data0, i_data1, i_data2, i_data3
$output v_position, v_normal, v_shadow

#include <bgfx.glsl>
#include <shadow.glsl>

void main() {
	mat4 model = mtxFromCols(i_data0, i_data1, i_data2, i_data3);
	v_position = mul(model, vec4(a_position, 1.0));
	v_normal = mul(model, vec4(a_normal, 0.0)).xyz;
	gl_Position = mul(u_viewProj, mul(model, vec4(a_position, 1.0)));
}
