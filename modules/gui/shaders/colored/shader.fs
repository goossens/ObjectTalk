//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_position, v_normal

#include <bgfx.glsl>
#include <light.glsl>

// main function
void main() {
	// return fragment color
	gl_FragColor = applyLight(u_material_color, v_position, v_normal);
}
