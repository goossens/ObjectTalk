//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_texcoord0

#include <bgfx_shader.glsl>
#include <utilities.glsl>

uniform vec4 u_linearize;
#define u_near u_linearize.x
#define u_far u_linearize.y

SAMPLER2D(s_texture, 0);

void main() {
	float depth = texture2D(s_texture, v_texcoord0).r;
	float linearized = linearizeDepth(depth, u_near, u_far) / u_far;
	gl_FragColor = vec4(linearized, 0.0, 0.0, 1.0);
}
