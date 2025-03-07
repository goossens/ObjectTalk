//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_texcoord0

#include <bgfx_shader.glsl>

uniform vec4 u_pixelate[1];
#define u_posterizeSize int(u_pixelate[0].x)

SAMPLER2D(s_texture, 0);

void main() {
	vec2 pixelCoord = v_texcoord0 * u_viewRect.zw;
	float x = int(pixelCoord.x) % u_posterizeSize;
	float y = int(pixelCoord.y) % u_posterizeSize;

	x = floor(u_posterizeSize / 2.0) - x;
	y = floor(u_posterizeSize / 2.0) - y;

	x = pixelCoord.x + x;
	y = pixelCoord.y + y;

	gl_FragColor = texture2D(s_texture, vec2(x, y) / u_viewRect.zw);
}
