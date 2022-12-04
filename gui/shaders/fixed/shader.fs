//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

#include <bgfx.glsl>

// uniforms
uniform vec4 u_material[1];

#define color u_material[0]

// main function
void main() {
	gl_FragColor = color;
}
