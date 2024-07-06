//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input a_position, a_texcoord0, i_data0, i_data1, i_data2, i_data3
$output v_texcoord0

#include <bgfx_shader.glsl>

void main() {
	v_texcoord0 = a_texcoord0;
	mat4 instance = mtxFromCols(i_data0, i_data1, i_data2, i_data3);
	mat4 model = mul(u_model[0], instance);
	vec3 pos = mul(model, vec4(a_position, 1.0)).xyz;
	gl_Position = mul(u_viewProj, vec4(pos, 1.0));
}
