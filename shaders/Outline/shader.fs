//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_texcoord0

#include <bgfx.glsl>

uniform vec4 u_outline;
#define u_pixelWidth u_outline.x
#define u_pixelHeight u_outline.y

SAMPLER2D(s_selectedTexture, 0);

void main() {
	float value = texture2D(s_selectedTexture, v_texcoord0).r;

	if (value > 0.0) {
		discard;
	}

	vec2 pixel = vec2(u_pixelWidth, u_pixelHeight);
	value += texture2D(s_selectedTexture, v_texcoord0 + vec2(-pixel.x, -pixel.y)).r;
	value += texture2D(s_selectedTexture, v_texcoord0 + vec2(0.0, -pixel.y)).r;
	value += texture2D(s_selectedTexture, v_texcoord0 + vec2(pixel.x, -pixel.y)).r;;
	value += texture2D(s_selectedTexture, v_texcoord0 + vec2(-pixel.x, 0.0)).r;
	value += texture2D(s_selectedTexture, v_texcoord0 + vec2(pixel.x, 0.0)).r;
	value += texture2D(s_selectedTexture, v_texcoord0 + vec2(-pixel.x, pixel.y)).r;
	value += texture2D(s_selectedTexture, v_texcoord0 + vec2(0.0, pixel.y)).r;
	value += texture2D(s_selectedTexture, v_texcoord0 + vec2(pixel.x, pixel.y)).r;

	if (value > 0.0) {
		value = 0.5 + value / 9.0;
	}

	gl_FragColor = vec4(1.0, 0.5, 0.0, value);
}
