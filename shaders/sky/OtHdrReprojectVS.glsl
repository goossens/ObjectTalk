//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input a_position
$output v_position

#include <bgfx_shader.glsl>

void main() {
	v_position = a_position;
	gl_Position = mul(u_viewProj, vec4(a_position, 1.0));
}
