//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_texcoord0

#include <bgfx.glsl>

uniform vec4 u_bloom;
#define u_pixelWidth u_bloom.x
#define u_pixelHeight u_bloom.y
#define u_intensity u_bloom.z

SAMPLER2D(s_bloomTexture, 0);

void main() {
	vec2 halfPixel = 0.5 * vec2(u_pixelWidth, u_pixelHeight);
	vec4 sum = vec4_splat(0.0);

	sum += (2.0 / 16.0) * texture2D(s_bloomTexture, v_texcoord0 + vec2(-halfPixel.x, 0.0));
	sum += (2.0 / 16.0) * texture2D(s_bloomTexture, v_texcoord0 + vec2(0.0, +halfPixel.y));
	sum += (2.0 / 16.0) * texture2D(s_bloomTexture, v_texcoord0 + vec2(+halfPixel.x, 0.0));
	sum += (2.0 / 16.0) * texture2D(s_bloomTexture, v_texcoord0 + vec2(0.0, -halfPixel.y));

	sum += (1.0 / 16.0) * texture2D(s_bloomTexture, v_texcoord0 + vec2(-halfPixel.x, -halfPixel.y));
	sum += (1.0 / 16.0) * texture2D(s_bloomTexture, v_texcoord0 + vec2(-halfPixel.x, +halfPixel.y));
	sum += (1.0 / 16.0) * texture2D(s_bloomTexture, v_texcoord0 + vec2(+halfPixel.x, -halfPixel.y));
	sum += (1.0 / 16.0) * texture2D(s_bloomTexture, v_texcoord0 + vec2(+halfPixel.x, +halfPixel.y));

	sum += (4.0 / 16.0) * texture2D(s_bloomTexture, v_texcoord0);

	gl_FragColor = u_intensity * sum;
}
