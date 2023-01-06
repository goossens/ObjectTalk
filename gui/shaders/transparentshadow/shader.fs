//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_texcoord0

#include <bgfx.glsl>

uniform vec4 u_scale;

// samplers
SAMPLER2D(s_texture, 1);

// main function
void main() {
	// get texture color
	vec4 color = texture2D(s_texture, v_texcoord0 / u_scale.r);

	// discard pixel if too transparent (it won't cast a shadow)
	if (color.w < 0.05) {
		discard;
	}
}
