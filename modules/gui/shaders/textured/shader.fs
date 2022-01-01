//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_position, v_normal, v_texcoord0

#include <bgfx.glsl>
#include <light.glsl>

SAMPLER2D(s_texture, 0);

// main function
void main() {
	// get texture color
	vec4 color = texture2D(s_texture, v_texcoord0);

	// return fragment color
	gl_FragColor = applyLight(color, v_position, v_normal);
}
