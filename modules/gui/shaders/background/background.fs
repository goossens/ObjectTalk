//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_position, v_texcoord0

#include <bgfx.glsl>

#include <material.glsl>

void main() {
	if (int(u_material_textured)) {
		gl_FragColor = texture2D(s_texture, v_texcoord0);

	} else {
		gl_FragColor = u_material_ambient + u_material_diffuse;
	}
}
