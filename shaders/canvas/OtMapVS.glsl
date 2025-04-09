//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input a_position, a_color0
$output v_color0

#include <bgfx_shader.glsl>

void main() {
	v_color0 = a_color0;
	gl_Position = mul(u_modelViewProj, vec4(a_position, 0.0, 1.0));
}
