//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input a_position, a_texcoord0
$output v_position, v_texcoord0

#include <bgfx_shader.glsl>

uniform vec4 u_viewSize;

void main() {
	v_position = a_position;
	v_texcoord0 = a_texcoord0;
	gl_Position = vec4(2.0 * v_position.x / u_viewSize.x - 1.0, 1.0 - 2.0 * v_position.y / u_viewSize.y, 0.0, 1.0);
}
