//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_normal, v_color0, v_data0

#include <bgfx_shader.glsl>

#include "common.glsl"

// main function
void main() {
	// store information in gbuffer
	gl_FragData[0] = vec4(mix(v_color0, v_color0 * 0.66, abs(v_data0.x)), 1.0);
	gl_FragData[1] = vec4((normalize(v_normal) * 0.5 + 0.5), 1.0);
	gl_FragData[2] = vec4(0.0, 1.0, 1.0, 1.0);
	gl_FragData[3] = vec4_splat(0.0);
}
