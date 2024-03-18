//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_texcoord0

#include <bgfx_shader.glsl>

#define FXAA_REDUCE_MIN (1.0 / 128.0)
#define FXAA_REDUCE_MUL (1.0 / 8.0)
#define FXAA_SPAN_MAX 8.0

uniform vec4 u_fxaa;
#define u_pixelWidth u_fxaa.x
#define u_pixelHeight u_fxaa.y

SAMPLER2D(s_postProcessTexture, 0);

void main() {
	// Fast Approximate Anti-Aliasing (FXAA)
	// Original FXAA algorithm by Timothy Lottes at Nvidia
	vec3 rgbNW = texture2D(s_postProcessTexture, v_texcoord0 + vec2(-u_pixelWidth, -u_pixelHeight)).rgb;
	vec3 rgbNE = texture2D(s_postProcessTexture, v_texcoord0 + vec2(u_pixelWidth, -u_pixelHeight)).rgb;
	vec3 rgbSW = texture2D(s_postProcessTexture, v_texcoord0 + vec2(-u_pixelWidth, u_pixelHeight)).rgb;
	vec3 rgbSE = texture2D(s_postProcessTexture, v_texcoord0 + vec2(u_pixelWidth, u_pixelHeight)).rgb;
	vec3 rgbM = texture2D(s_postProcessTexture, v_texcoord0).rgb;
	vec3 luma = vec3(0.299, 0.587, 0.114);

	float lumaNW = dot(rgbNW, luma);
	float lumaNE = dot(rgbNE, luma);
	float lumaSW = dot(rgbSW, luma);
	float lumaSE = dot(rgbSE, luma);
	float lumaM = dot(rgbM, luma);
	float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
	float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

	vec2 dir = vec2(-((lumaNW + lumaNE) - (lumaSW + lumaSE)), (lumaNW + lumaSW) - (lumaNE + lumaSE));
	float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN);
	float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);
	dir = min(vec2(FXAA_SPAN_MAX, FXAA_SPAN_MAX), max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX), dir * rcpDirMin)) * vec2(u_pixelWidth, u_pixelHeight);

	vec3 rgbA = 0.5 * (
		texture2D(s_postProcessTexture, v_texcoord0 + dir * (1.0 / 3.0 - 0.5)).xyz +
		texture2D(s_postProcessTexture, v_texcoord0 + dir * (2.0 / 3.0 - 0.5)).xyz);

	vec3 rgbB = rgbA * 0.5 + 0.25 * (
		texture2D(s_postProcessTexture, v_texcoord0 + dir * -0.5).xyz +
		texture2D(s_postProcessTexture, v_texcoord0 + dir * 0.5).xyz);

	float lumaB = dot(rgbB, luma);

	if ((lumaB < lumaMin) || (lumaB > lumaMax)) {
		gl_FragColor = vec4(rgbA, 1.0);

	} else {
		gl_FragColor = vec4(rgbB, 1.0);
	}
}
