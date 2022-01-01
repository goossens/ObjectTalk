//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input a_position, a_normal
$output v_position_world, v_position_camera, v_normal_world, v_normal_camera

#include <bgfx.glsl>
#include <material.glsl>

void main() {
	v_position_world = mul(u_model[0], vec4(a_position, 1.0)).xyz;
	v_position_camera = mul(u_modelView, vec4(a_position, 1.0)).xyz;
	v_normal_world = normalize(mul(u_model[0], vec4(a_normal, 0.0))).xyz;
	v_normal_camera = normalize(mul(u_modelView, vec4(a_normal, 0.0))).xyz;

	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));
}
