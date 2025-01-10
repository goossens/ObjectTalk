//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_texcoord0

#include <bgfx_shader.glsl>

SAMPLER2D(s_texture, 0);

void main() {
	vec4 sum = vec4_splat(0.0);

	for (int y = -3; y < 3; y++) {
		for (int x = -3; x < 3; x++) {
			sum += texture2D(s_texture, v_texcoord0 + vec2(x, y) * u_viewTexel.xy) * 0.25;
		}
	}

	vec4 color = texture2D(s_texture, v_texcoord0);
	float brightness = (color.r + color.g + color.b) / 3.0;

	if (brightness < 0.3) {
		gl_FragColor = sum * sum * 0.012 + color;

	} else if (brightness < 0.5) {
		gl_FragColor = sum * sum * 0.009 + color;

	} else {
		gl_FragColor = sum * sum * 0.0075 + color;
	}
}
