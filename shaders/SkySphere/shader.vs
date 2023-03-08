//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input a_position, a_texcoord0
$output v_texcoord0

#include <bgfx.glsl>

void main() {
	v_texcoord0 = vec2(1.0 - a_texcoord0.x, a_texcoord0.y);
	vec4 pos = mul(u_modelViewProj, vec4(a_position, 1.0));
	pos.z = pos.w;
	gl_Position = pos;
}
