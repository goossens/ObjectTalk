//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input a_position
$output v_texcoord0

#include <bgfx.glsl>

void main() {
	v_texcoord0 = a_position;
	gl_Position = mul(mul(u_proj, mat4(mat3(u_view))), vec4(a_position, 1.0));
	gl_Position.z = gl_Position.w;
}
