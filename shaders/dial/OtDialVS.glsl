//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input a_position, a_texcoord0
$output v_texcoord0

#include <bgfx_shader.glsl>

void main() {
	v_texcoord0 = a_texcoord0;
	vec4 pos = mul(u_modelViewProj, vec4(a_position, 0.0, 1.0));
	gl_Position = vec4(pos.xy, 0.0, 1.0);
}
