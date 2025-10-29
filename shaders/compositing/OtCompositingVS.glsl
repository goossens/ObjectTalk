//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$output v_texcoord0

#include <bgfx_shader.glsl>

void main() {
	v_texcoord0 = vec2((gl_VertexID << 1) & 2, gl_VertexID & 2);
	gl_Position = vec4(v_texcoord0 * vec2(2.0, -2.0) + vec2(-1.0, 1.0), 0.0, 1.0);
}
