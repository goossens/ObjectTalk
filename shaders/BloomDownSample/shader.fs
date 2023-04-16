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

SAMPLER2D(s_bloomTexture, 0);

void main() {
	vec2 onePixel = vec2(u_pixelWidth, u_pixelHeight);
	vec2 halfPixel = 0.5 * onePixel;
	vec3 sum = vec3_splat(0.0);

	sum += (4.0 / 32.0) * texture2D(s_bloomTexture, v_texcoord0).rgb;

	sum += (4.0 / 32.0) * texture2D(s_bloomTexture, v_texcoord0 + vec2(-halfPixel.x, -halfPixel.y)).rgb;
	sum += (4.0 / 32.0) * texture2D(s_bloomTexture, v_texcoord0 + vec2(+halfPixel.x, +halfPixel.y)).rgb;
	sum += (4.0 / 32.0) * texture2D(s_bloomTexture, v_texcoord0 + vec2(+halfPixel.x, -halfPixel.y)).rgb;
	sum += (4.0 / 32.0) * texture2D(s_bloomTexture, v_texcoord0 + vec2(-halfPixel.x, +halfPixel.y)).rgb;

	sum += (2.0 / 32.0) * texture2D(s_bloomTexture, v_texcoord0 + vec2(+onePixel.x, 0.0)).rgb;
	sum += (2.0 / 32.0) * texture2D(s_bloomTexture, v_texcoord0 + vec2(-onePixel.x, 0.0)).rgb;
	sum += (2.0 / 32.0) * texture2D(s_bloomTexture, v_texcoord0 + vec2(0.0, +onePixel.y)).rgb;
	sum += (2.0 / 32.0) * texture2D(s_bloomTexture, v_texcoord0 + vec2(0.0, -onePixel.y)).rgb;

	sum += (1.0 / 32.0) * texture2D(s_bloomTexture, v_texcoord0 + vec2(+onePixel.x, +onePixel.y)).rgb;
	sum += (1.0 / 32.0) * texture2D(s_bloomTexture, v_texcoord0 + vec2(-onePixel.x, +onePixel.y)).rgb;
	sum += (1.0 / 32.0) * texture2D(s_bloomTexture, v_texcoord0 + vec2(+onePixel.x, -onePixel.y)).rgb;
	sum += (1.0 / 32.0) * texture2D(s_bloomTexture, v_texcoord0 + vec2(-onePixel.x, -onePixel.y)).rgb;

	gl_FragColor = vec4(sum, 1.0);
}
