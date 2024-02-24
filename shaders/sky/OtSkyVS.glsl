//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input a_position, a_texcoord0
$output v_position

#include <bgfx_shader.glsl>
#include <utilities.glsl>

uniform mat4 u_skyInvViewProjUniform;

void main() {
	vec4 pos = mul(u_skyInvViewProjUniform, vec4(uvToClipSpace(a_texcoord0, 1.0), 1.0));
	v_position = pos.xyz / pos.w;
	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));
}
