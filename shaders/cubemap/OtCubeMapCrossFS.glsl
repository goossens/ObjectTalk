//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_texcoord0

#include <bgfx_shader.glsl>

// uniforms
uniform vec4 u_crossUniform[1];
#define u_mipLevel u_crossUniform[0].x

// samplers
SAMPLERCUBE(s_texColor, 0);

// functions
void main() {
	gl_FragColor = textureCubeLod(s_texColor, v_texcoord0, u_mipLevel);
}
