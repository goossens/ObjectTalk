//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input a_position, a_normal, a_texcoord0
$output v_position, v_normal, v_texcoord0, v_shadow

#include <bgfx.glsl>
#include <material.glsl>
#include <shadow.glsl>

void main() {
	v_position = mul(u_model[0], vec4(a_position, 1.0));
	v_normal = mul(u_model[0], vec4(a_normal, 0.0)).xyz;
	v_texcoord0 = mul(u_uv_transform, vec3(a_texcoord0, 1.0)).xy;
	v_shadow = mul(u_shadowMatrix, mul(u_model[0], vec4(a_position, 1.0)));
	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));
}
