//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_texcoord0

#include <bgfx_shader.glsl>

uniform vec4 u_picking[1];
#define u_entityID u_picking[0].r

void main() {
	gl_FragColor = vec4_splat(u_entityID);
}
