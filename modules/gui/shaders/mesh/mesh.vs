//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input a_position, a_normal, a_texcoord0, a_color0
$output v_position, v_normal, v_texcoord0, v_color0

#include <bgfx_shader.sh>

void main() {
	v_position = mul(u_modelView, vec4(a_position, 1.0));
	v_normal = normalize(mul(u_modelView, vec4(a_normal, 0.0)));
	v_texcoord0 = a_texcoord0;
	v_color0 = a_color0;
	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));
}
