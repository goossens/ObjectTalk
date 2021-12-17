//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input a_position, a_texcoord0, a_color0
$output v_color0, v_texcoord0

#include <bgfx.glsl>

void main() {
	v_texcoord0 = a_texcoord0;
	v_color0 = a_color0;
	vec4 pos = mul(u_viewProj, vec4(a_position.xy, 0.0, 1.0) );
	gl_Position = vec4(pos.x, pos.y, 0.0, 1.0);
}
