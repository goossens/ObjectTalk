//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input a_position
$output v_position_world, v_position_view, v_position_screen, v_normal, v_tangent, v_bitangent

#include <bgfx.glsl>

void main() {
	vec4 position = mul(u_model[0], vec4(a_position, 1.0));
	v_position_world = position.xyz;
	v_position_view = mul(u_modelView, vec4(a_position, 1.0)).xyz;
	v_position_screen = mul(u_modelViewProj, vec4(a_position, 1.0));

	v_normal = mul(u_modelView, vec4(0.0, 1.0, 0.0, 0.0)).xyz;
	v_tangent = mul(u_modelView, vec4(1.0, 0.0, 0.0, 0.0)).xyz;
	v_bitangent = mul(u_modelView, vec4(0.0, 0.0, 1.0, 0.0)).xyz;

	gl_Position = v_position_screen;
}
