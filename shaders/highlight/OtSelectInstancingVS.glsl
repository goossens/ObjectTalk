//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input a_position, i_data0, i_data1, i_data2, i_data3

#include <bgfx_shader.glsl>

void main() {
	mat4 model = u_model[0] * mtxFromCols(i_data0, i_data1, i_data2, i_data3);
	gl_Position = mul(model * u_viewProj, vec4(a_position, 1.0));
}
