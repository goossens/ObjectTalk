//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

// Based on https://en.wikipedia.org/wiki/Alpha_compositing

$input v_texcoord0

#include <bgfx_shader.glsl>

SAMPLER2D(s_texture, 0);
SAMPLER2D(s_overlay, 1);

void main() {
	vec4 bg = texture2D(s_texture, v_texcoord0);
	vec4 fg = texture2D(s_overlay, v_texcoord0);
	float inv = 1.0 - fg.a;
	float alpha = fg.a + bg.a * inv;

	gl_FragColor = vec4(
		(fg.r * fg.a + bg.r * bg.a * inv) / alpha,
		(fg.g * fg.a + bg.g * bg.a * inv) / alpha,
		(fg.b * fg.a + bg.b * bg.a * inv) / alpha,
		alpha);
}
