//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input a_position, a_normal, a_tangent, a_bitangent, a_texcoord0
$output v_position, v_normal, v_tangent, v_bitangent, v_texcoord0

#include <bgfx.glsl>

void main() {
	v_texcoord0 = a_texcoord0;
	v_position = mul(u_model[0], vec4(a_position, 1.0)).xyz;
	v_normal = mul(u_model[0], vec4(a_normal, 0.0)).xyz;
	v_tangent = mul(u_model[0], vec4(a_tangent, 0.0)).xyz;
	v_bitangent = mul(u_model[0], vec4(a_bitangent, 0.0)).xyz;
	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));
}
