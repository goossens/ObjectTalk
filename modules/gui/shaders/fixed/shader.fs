//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

#include <bgfx.glsl>
#include <material.glsl>

// main function
void main() {
	// return fixed color
	gl_FragColor = u_material_color;
}
