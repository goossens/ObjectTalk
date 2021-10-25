//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input a_position
$output v_position_world, v_position_camera, v_position_screen, v_normal, v_tangent, v_bitangent

#include <bgfx.glsl>

void main() {
	v_position_world = mul(u_model[0], vec4(a_position, 1.0)).xyz;
	v_position_camera = mul(u_modelView, vec4(a_position, 1.0)).xyz;
	v_position_screen = mul(u_modelViewProj, vec4(a_position, 1.0));

	v_normal = normalize(mul(u_modelView, vec4(0.0, 1.0, 0.0, 0.0))).xyz;
	v_tangent = normalize(mul(u_modelView, vec4(1.0, 0.0, 0.0, 0.0))).xyz;
	v_bitangent = normalize(mul(u_modelView, vec4(0.0, 0.0, 1.0, 0.0))).xyz;

	gl_Position = v_position_screen;
}
