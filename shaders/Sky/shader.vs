//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input a_position
$output v_position

#include <bgfx.glsl>

void main() {
	v_position = a_position;
	vec4 pos = mul(u_modelViewProj, vec4(a_position, 1.0));
	pos.z = pos.w;
	gl_Position = pos;
}
