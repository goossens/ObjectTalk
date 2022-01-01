//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input a_position, a_normal, a_texcoord0
$output v_position, v_normal, v_texcoord0

#include <bgfx.glsl>
#include <material.glsl>

void main() {
	v_position = mul(u_modelView, vec4(a_position, 1.0)).xyz;
	v_normal = normalize(mul(u_modelView, vec4(a_normal, 0.0))).xyz;
	v_texcoord0 = mul(u_uv_transform, vec3(a_texcoord0, 1.0)).xy;
	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));
}
