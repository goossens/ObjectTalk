//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input a_position, a_normal
$output v_position, v_normal

#include <bgfx.glsl>

void main() {
	v_position = mul(u_modelView, vec4(a_position, 1.0));
	v_normal = normalize(mul(u_modelView, vec4(a_normal, 0.0)));
	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));
}