//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_position, v_normal, v_texcoord0, v_shadow

#include <bgfx.glsl>
#include <light.glsl>

uniform vec4 u_blendmap[1];
#define u_blendmapScale u_blendmap[0].x

SAMPLER2D(s_texture_m, 1);
SAMPLER2D(s_texture_n, 2);
SAMPLER2D(s_texture_r, 3);
SAMPLER2D(s_texture_g, 4);
SAMPLER2D(s_texture_b, 5);

// main function
void main() {
	// blend texture colors based on blendmap
	vec4 blend = texture2D(s_texture_m, v_texcoord0);
	float b = 1.0 - blend.r - blend.g - blend.b;
	vec2 tiled = v_texcoord0 * u_blendmapScale;

	vec4 color = texture2D(s_texture_n, tiled) * b +
		texture2D(s_texture_r, tiled) * blend.r +
		texture2D(s_texture_g, tiled) * blend.g +
		texture2D(s_texture_b, tiled) * blend.b;

	// return fragment color
	gl_FragColor = applyLight(color, v_position, v_normal, v_shadow);
}
