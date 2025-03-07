//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_texcoord0

#include <bgfx_shader.glsl>

uniform vec4 u_posterize[1];
#define u_levels u_posterize[0].x

SAMPLER2D(s_texture, 0);

void main() {
	vec4 color = texture2D(s_texture, v_texcoord0);
	float greyscale = 0.2125 * color.r + 0.7154 * color.g + 0.0721 * color.b;

	float lower = floor(greyscale * u_levels) / u_levels;
	float upper = ceil(greyscale * u_levels) / u_levels;

	float level = (abs(greyscale - lower) <= abs(upper - greyscale)) ? lower : upper;
	float adjustment = level / greyscale;

	gl_FragColor = vec4(color.rgb * adjustment, color.a);
}
