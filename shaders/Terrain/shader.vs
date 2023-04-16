//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input a_position, a_normal
$output v_position, v_normal

#include <bgfx.glsl>

void main() {
	v_position = mul(u_model[0], vec4(a_position, 1.0)).xyz;
	v_normal = mul(u_model[0], vec4(a_normal, 0.0)).xyz;
	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));
}
