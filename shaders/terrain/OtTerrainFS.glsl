//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

#include <bgfx_shader.glsl>

// main function
void main() {
	// store information in gbuffer
	gl_FragData[0] = vec4(1.0, 0.0, 1.0, 1.0);
	gl_FragData[1] = vec4(0.0, 0.0, 1.0, 0.0);
	gl_FragData[2] = vec4(0.0, 1.0, 1.0, 0.0);
	gl_FragData[3] = vec4_splat(0.0);
}
